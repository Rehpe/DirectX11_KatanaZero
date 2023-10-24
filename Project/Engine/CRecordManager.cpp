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

	if (m_PlayMode == (UINT)PLAYMODE::FORWARD)	// ������ ���
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

	for (size_t i = 0; i < 31; i++)	// 31�� UI ���̾�� ����
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
	// ��� �ߴ�
	EndRecording();

	m_bFinishPlay = false;

	if (m_iCurFrm >= m_iMaxFrame)
	{
		m_iCurFrm = m_iMaxFrame;
		m_bFinishPlay = true;
		return;
	}

	// m_iCurFrm ��° �����ӿ� ����� ��� ���� ������ ã��
	auto range = m_mapRecordData.equal_range(m_iCurFrm);

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		const tGameObjData& objData = iter->second;
		CGameObject* pObj = CLevelManager::GetInst()->FindObjectByID(objData.ID);
		// �ش� ������Ʈ�� ���� ���(������ ó������ ������ �����ϴ� ������Ʈ)
		if (pObj)
		{
			pObj->ApplyObjData(objData);
		}
		// �ش� ������Ʈ�� ���� ���
		else if (pObj == nullptr)
		{
			int NewID = 0;
			CGameObject* pNewObj = nullptr;
			NewID = m_OriginalID[objData.ID];
			if (NewID != 0)
			{
				pNewObj = CLevelManager::GetInst()->FindObjectByID(NewID);
			}
			// CreatePrefab���� NewID�� �ο��޾Ҵ� ��ü�� ���
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
				// �ش� ������Ʈ�� ���� ��� -->��Ÿ�� �� �������� ������ ������Ʈ
				// �̸����� �������� �ҷ��� �����Ѵ�.
				wstring Name = objData.ObjName;
				wstring PrefKey = L"prefab\\" + Name + L".pref";
				Ptr<CPrefab> Prefab = CResourceManager::GetInst()->FindRes<CPrefab>(PrefKey);
				if (Prefab != nullptr)
				{
					pObj = Prefab->Instantiate();
					// ó�� ����� ����̹Ƿ� objData.ID�� ���ο� ID�� ¦���� �ʿ� ����
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
	// ��� �ߴ�
	EndRecording();

	m_bFinishPlay = false;

	// PostProcess (�۸�ġ) ����
	int Speed = 3;

	if (m_iCurFrm <= 1 * Speed)
	{
		m_iCurFrm = 0;
		m_bFinishPlay = true;
		return;
	}

	// m_iCurFrm ��° �����ӿ� ����� ��� ���� ������ ã��
	auto range = m_mapRecordData.equal_range(m_iCurFrm - 1);

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		const tGameObjData& objData = iter->second;
		CGameObject* pObj = CLevelManager::GetInst()->FindObjectByID(objData.ID);

		// �ش� ������Ʈ�� ���� ���(������ ó������ ������ �����ϴ� ������Ʈ)
		if (pObj)
		{
			pObj->ApplyObjData(objData);
		}
		// �ش� ������Ʈ�� ���� ���
		else if (pObj == nullptr)
		{
			int NewID = 0;
			CGameObject* pNewObj = nullptr;
			NewID = m_OriginalID[objData.ID];
			if (NewID != 0)
			{
				pNewObj = CLevelManager::GetInst()->FindObjectByID(NewID);
			}
			// CreatePrefab���� NewID�� �ο��޾Ҵ� ��ü�� ���
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
				// �ش� ������Ʈ�� ���� ��� -->��Ÿ�� �� �������� ������ ������Ʈ

				// �̸����� �������� �ҷ��� �����Ѵ�.
				wstring Name = objData.ObjName;
				wstring PrefKey = L"prefab\\" + Name + L".pref";
				Ptr<CPrefab> Prefab = CResourceManager::GetInst()->FindRes<CPrefab>(PrefKey);
				if (Prefab != nullptr)
				{
					pObj = Prefab->Instantiate();
					// ó�� ����� ����̹Ƿ� objData.ID�� ���ο� ID�� ¦���� �ʿ� ����
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
