#include "pch.h"
#include "CRecordManager.h"

#include "CTimeManager.h"
#include "CResourceManager.h"
#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CPrefab.h"

CRecordManager::CRecordManager()
	: m_iCurFrm(0)
	, m_PlayMode(0)
	, m_bFinishPlay(false)
	, m_iMaxFrame(0)
{
}

CRecordManager::~CRecordManager()
{
	ResetRecordData();
}

void CRecordManager::init()
{
	m_iFrameCounter = 0;
}

void CRecordManager::tick()
{
	m_iFrameCounter++;

	int recordInterval = static_cast<int>(1.0f / CTimeManager::GetInst()->GetTimeFactor());

	if (m_iFrameCounter >= recordInterval)
	{
		m_iFrameCounter = 0; // Reset the counter

		if (m_bRecord)
		{
			RecordLevelData();
			++m_iCurFrm;
		}
	}

	if (m_PlayMode == (UINT)PLAYMODE::FORWARD)	// 정방향 재생
	{
		Playback();
	}
	else if (m_PlayMode == (UINT)PLAYMODE::BACKWARD)
	{
		ReversePlayback();
	}
}

void CRecordManager::RecordLevelData()
{
	CLevel* CurLevel = CLevelManager::GetInst()->GetCurLevel();

	for (size_t i = 0; i < 31; i++)	// 31번 UI 레이어는 제외
	{
		vector<CGameObject*> LayerObjs = CurLevel->GetLayer(i)->GetObjects();

		for (size_t j = 0; j < LayerObjs.size(); j++)
		{
			tGameObjData objData = LayerObjs[j]->RecordObjData();
			m_mapRecordData.insert(make_pair(m_iCurFrm, objData));
		}
	}

	m_iMaxFrame = max(m_iMaxFrame, m_iCurFrm);
}

void CRecordManager::Playback()
{
	// 기록 중단
	EndRecording();

	m_bFinishPlay = false;

	if (m_iCurFrm >= m_iMaxFrame)
	{
		m_iCurFrm = m_iMaxFrame;
		m_bFinishPlay = true;
		return;
	}

	// m_iCurFrm 번째 프레임에 저장된 모든 게임 데이터 찾기
	auto range = m_mapRecordData.equal_range(m_iCurFrm);

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		const tGameObjData& objData = iter->second;
		CGameObject* pObj = CLevelManager::GetInst()->FindObjectByID(objData.ID);
		// 해당 오브젝트가 있을 경우(레벨에 처음부터 끝까지 존재하는 오브젝트)
		if (pObj)
		{
			pObj->ApplyObjData(objData);
		}
		// 해당 오브젝트가 없는 경우
		else if (pObj == nullptr)
		{
			int NewID = 0;
			CGameObject* pNewObj = nullptr;
			NewID = m_OriginalID[objData.ID];
			if (NewID != 0)
			{
				pNewObj = CLevelManager::GetInst()->FindObjectByID(NewID);
			}
			// CreatePrefab으로 NewID를 부여받았던 객체인 경우
			if (pNewObj)
			{
				if (objData.IsDead)
				{
					DestroyObject(pNewObj);
				}
				pNewObj->ApplyObjData(objData);
			}
			else
			{
				// 해당 오브젝트가 없을 경우 -->런타임 중 동적으로 생성된 오브젝트
				// 이름으로 프리팹을 불러와 생성한다.
				wstring Name = objData.ObjName;
				wstring PrefKey = L"prefab\\" + Name + L".pref";
				Ptr<CPrefab> Prefab = CResourceManager::GetInst()->FindRes<CPrefab>(PrefKey);
				if (Prefab != nullptr)
				{
					pObj = Prefab->Instantiate();
					// 처음 만드는 경우이므로 objData.ID와 새로운 ID를 짝지어 맵에 저장
					m_OriginalID[objData.ID] = pObj->GetID();
					SpawnGameObject(pObj, Vec3(0.f, 0.f, 0.f), L"Effect");
					pObj->ApplyObjData(objData);
				}
			}
		}
	}

	m_iCurFrm++;
}

void CRecordManager::ReversePlayback()
{
	// 기록 중단
	EndRecording();

	m_bFinishPlay = false;

	// PostProcess (글리치) 생성
	int Speed = 3;

	if (m_iCurFrm <= 1 * Speed)
	{
		m_iCurFrm = 0;
		m_bFinishPlay = true;
		return;
	}

	// m_iCurFrm 번째 프레임에 저장된 모든 게임 데이터 찾기
	auto range = m_mapRecordData.equal_range(m_iCurFrm - 1);

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		const tGameObjData& objData = iter->second;
		CGameObject* pObj = CLevelManager::GetInst()->FindObjectByID(objData.ID);

		// 해당 오브젝트가 있을 경우(레벨에 처음부터 끝까지 존재하는 오브젝트)
		if (pObj)
		{
			pObj->ApplyObjData(objData);
		}
		// 해당 오브젝트가 없는 경우
		else if (pObj == nullptr)
		{
			int NewID = 0;
			CGameObject* pNewObj = nullptr;
			NewID = m_OriginalID[objData.ID];
			if (NewID != 0)
			{
				pNewObj = CLevelManager::GetInst()->FindObjectByID(NewID);
			}
			// CreatePrefab으로 NewID를 부여받았던 객체인 경우
			if (pNewObj)
			{
				if (objData.IsNewlyCreated)
				{
					DestroyObject(pNewObj);
				}
				pNewObj->ApplyObjData(objData);
			}
			else
			{
				// 해당 오브젝트가 없을 경우 -->런타임 중 동적으로 생성된 오브젝트

				// 이름으로 프리팹을 불러와 생성한다.
				wstring Name = objData.ObjName;
				wstring PrefKey = L"prefab\\" + Name + L".pref";
				Ptr<CPrefab> Prefab = CResourceManager::GetInst()->FindRes<CPrefab>(PrefKey);
				if (Prefab != nullptr)
				{
					pObj = Prefab->Instantiate();
					// 처음 만드는 경우이므로 objData.ID와 새로운 ID를 짝지어 맵에 저장
					m_OriginalID[objData.ID] = pObj->GetID();
					SpawnGameObject(pObj, Vec3(0.f, 0.f, 0.f), L"Effect");
					pObj->ApplyObjData(objData);
				}
			}
		}

	}

	m_iCurFrm -= Speed;
}

void CRecordManager::ResetRecordData()
{
	m_iCurFrm = 0;
	m_iMaxFrame = 0;
	m_mapRecordData.clear();
	m_OriginalID.clear();
	m_bFinishPlay = false;
	m_PlayMode = (UINT)PLAYMODE::NONE;
}
