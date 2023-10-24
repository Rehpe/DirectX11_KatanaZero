#include "pch.h"
#include "CGameManagerScript.h"

#include <Engine\CKeyManager.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CRecordManager.h>
#include <Engine\CAnim2D.h>
#include <Engine\CEventManager.h>

#include "CPlayerScript.h"
#include "CEnemyScript.h"
#include "CWallScript.h"
#include "CLevelSaveLoad.h"


CGameManagerScript::CGameManagerScript()
	:CScript((UINT)SCRIPT_TYPE::GAMEMANAGERSCRIPT)
	, m_bIsAllEnemyDead(false)
	, m_fTimer(0.f)
	, m_fTimeLimit(100.f)
	, m_fSlowModeCoolDown(11.f)
	, m_fSlowModeTimer(11.f)
	, m_fMapOpacity(0.f)
	, m_fFadeOpacity(0.f)
	, m_bInit(false)
	, m_iLevelIdx(0)
	, m_StageNoti(nullptr)
	, m_EndWall(nullptr)
	, m_PostProcess(nullptr)
	, m_FadeInOut(nullptr)
{
	CLevelManager::GetInst()->RegisterGameManagerScript(this);
	

	m_LevelPath.push_back(L"level\\TestLevel.lv");
	m_LevelPath.push_back(L"level\\TestLevel2.lv");
	m_LevelPath.push_back(L"level\\TestLevel3.lv");
	
}

CGameManagerScript::~CGameManagerScript()
{
}

void CGameManagerScript::begin()
{
	m_bInit = true;

	CLevelManager::GetInst()->RegisterGameManager(GetOwner());

	GetCurLevelIdx();

	// 게임 진행시간 초기화
	m_fTimeLimit = 100.f;
	m_fTimer = m_fTimeLimit;

	// SlowModeCoolDown 초기화
	m_fSlowModeTimer = 11.f;

	// RecordManager 기록 시작
	CRecordManager::GetInst()->ResetRecordData();
	CRecordManager::GetInst()->StartRecording();
}

void CGameManagerScript::tick()
{
	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* Player = Level->FindObjectByName(L"Player");
	if (Player == nullptr)
		return;

	if (Level->GetState() == LEVEL_STATE::PLAY && m_bInit)
	{
		init();
		return;
	}
	
	// 제한 시간 
	m_fTimer -= DT;
	if (m_fTimer <= 0)
	{
		m_fTimer = 0.f;
		// 게임 오버 및 재시작
	}

	// Pause창 켜기
	if (KEY_TAP(KEY::ESC))
	{
		//Level->ChangeState(LEVEL_STATE::PAUSE);

		// 재시작

		// 메인 메뉴

		// 치트 모드
	}

	// 슬로우 모드
	if ((m_fSlowModeTimer > 0.f) && (KEY_PRESSED(KEY::LSHIFT)))
	{
		CTimeManager::GetInst()->SetTimeFactor(0.2f);
		m_fSlowModeTimer -= DT * (1.f / 0.2f);
		FadeOut(Level->FindObjectByName(L"BlackOverlay"), m_fMapOpacity, 0.8f, 15.f);
		int OverlayUse = 1;
		float OverlayAlpha = 0.8f;
		Vec4 BrightBlue = Vec4{ 0.0f, 0.5f, 1.0f, 1.0f }; // BrightBlue
		Player->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &BrightBlue);
		Player->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &OverlayUse);
		Player->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &OverlayAlpha);

		if (!m_bSlowActivated)
		{
			// Sound Effect
			Ptr<CSound> SlowOnSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\SlowModeOn.ogg");
			SlowOnSFX->Play(1, 1.f, 0.5f, false);
			
			m_bSlowActivated = true;
		}
	}
	else if (KEY_RELEASED(KEY::LSHIFT))
	{
		m_bSlowActivated = false;

		CTimeManager::GetInst()->ResetTimeFactor();
		int OverlayUse = 0;
		Player->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &OverlayUse);

		// Sound Effect
		Ptr<CSound> SlowOffSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\SlowModeOff.wav");
		SlowOffSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
	}
	if (!(KEY_PRESSED(KEY::LSHIFT) && m_fMapOpacity != 0.f))
	{
		FadeOut(Level->FindObjectByName(L"BlackOverlay"), m_fMapOpacity, 0.f, 15.f);
		m_fSlowModeTimer += DT;
		if (m_fSlowModeTimer >= 11.f)
			m_fSlowModeTimer = 11.f;
	}

	// 플레이어 죽음 감지
	CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
	if (PlayerScript->IsDead())
	{
		if (CRecordManager::GetInst()->GetPlayMode() == (UINT)PLAYMODE::NONE)
		{
			// 녹화 종료
			CRecordManager::GetInst()->EndRecording();

			// StageNoti에 Level Fail 창 띄우기
			Ptr<CTexture> LevelFailTex = CResourceManager::GetInst()->FindRes<CTexture>(L"texture\\LevelFail.png");
			m_StageNoti->MeshRender()->GetMaterial()->SetTexParam(TEX_0, LevelFailTex);

			if (KEY_TAP(KEY::SPACE))
			{
				// 역재생
				CRecordManager::GetInst()->SetPlayMode((UINT)PLAYMODE::BACKWARD);

				// Glitch PostProcess
				m_PostProcess = Level->FindObjectByName(L"PostProcess");
				m_PostProcess->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"NewDistortionMtrl"));

				// Sound Effect
				Ptr<CSound> RewindSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Rewind.wav");
				RewindSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
			}
		}
		else
		{
			if (m_StageNoti == nullptr)
				return;
			// Level Fail 창 끄기
			m_StageNoti->MeshRender()->GetMaterial()->SetTexParam(TEX_0, nullptr);
		}

		if (CRecordManager::GetInst()->GetCurFrm() == 0)
		{
			// Glitch PostProcess 초기화
			m_PostProcess->MeshRender()->SetMaterial(nullptr);

			CRecordManager::GetInst()->ResetRecordData();

			Level->ChangeState(LEVEL_STATE::STOP);

			// 다시 불러오기
			CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(m_LevelPath[m_iLevelIdx]);
			tEvent evn = {};
			evn.Type = EVENT_TYPE::LEVEL_CHANGE;
			evn.wParam = (DWORD_PTR)pLoadedLevel;
			CEventManager::GetInst()->AddEvent(evn);
		}
	}

	vector<CGameObject*> Enemys = Level->FindLayerByName(L"Monster")->GetParentObject();

	for (size_t i = 0; i < Enemys.size(); i++)
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(Enemys[i]->GetScript<CEnemyScript>());
		if (EnemyScript != nullptr)
		{
			if (!EnemyScript->IsDead())
			{
				m_bIsAllEnemyDead = false;
				break;
			}
			m_bIsAllEnemyDead = true;
		}
	}

	if (m_bIsAllEnemyDead)
	{
		// 레벨 내의 모든 적이 죽었을 경우 EndWall 충돌체 Off / ToNextLevel의 충돌체 On
		CGameObject* NextLevel = Level->FindObjectByName(L"ToNextLevel");
		NextLevel->Collider2D()->SetColliderActivate(true);
		if (m_EndWall != nullptr)
			m_EndWall->Collider2D()->SetColliderActivate(false);
	}

	if (m_bLevelClear)
	{	
		if (CRecordManager::GetInst()->GetPlayMode() == (UINT)PLAYMODE::NONE)
		{
			// 녹화 종료
			CRecordManager::GetInst()->EndRecording();

			// Fade out
			UIFadeOut();

			if (IsFadeOutEnd())
			{
				// StageNoti에 Level Clear 창 띄우기
				Ptr<CTexture> LevelClearTex = CResourceManager::GetInst()->FindRes<CTexture>(L"texture\\LevelClear.png");
				m_StageNoti->MeshRender()->GetMaterial()->SetTexParam(TEX_0, LevelClearTex);

				if (KEY_TAP(KEY::SPACE))
				{
					// Sound Effect
					Ptr<CSound> TransitionSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Transition.wav");
					TransitionSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);

					if (CRecordManager::GetInst()->GetPlayMode() == (UINT)PLAYMODE::NONE)
					{
						CRecordManager::GetInst()->ResetFrame();
						CRecordManager::GetInst()->SetPlayMode((UINT)PLAYMODE::FORWARD);
					}

					// Glitch PostProcess
					m_PostProcess = Level->FindObjectByName(L"PostProcess");
					m_PostProcess->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"GlitchMtrl"));
				}
			}
		}
		else
		{
			// Level Clear 창 끄기
			m_StageNoti->MeshRender()->GetMaterial()->SetTexParam(TEX_0, nullptr);

			UIFadeIn();
		}

		if (CRecordManager::GetInst()->IsFinishPlaying())
		{
			if (KEY_TAP(KEY::SPACE))
			{
				UIFadeOut();
				
				m_iLevelIdx++;

				//Level->ChangeState(LEVEL_STATE::STOP);
				
				if (m_iLevelIdx >= m_LevelPath.size())
				{
					CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"level\\Title.lv");
					tEvent evn = {};
					evn.Type = EVENT_TYPE::LEVEL_CHANGE;
					evn.wParam = (DWORD_PTR)pLoadedLevel;
					CEventManager::GetInst()->AddEvent(evn);

					CRecordManager::GetInst()->ResetRecordData();

					Ptr<CSound> BGM = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\BGM.ogg");
					BGM->Stop();

					CLevelManager::GetInst()->RegisterGameManagerScript(nullptr);
					
					return;
				}

				CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(m_LevelPath[m_iLevelIdx]);
				tEvent evn = {};
				evn.Type = EVENT_TYPE::LEVEL_CHANGE;
				evn.wParam = (DWORD_PTR)pLoadedLevel;
				CEventManager::GetInst()->AddEvent(evn);

				CRecordManager::GetInst()->ResetRecordData();
			}
		}
	}
}

void CGameManagerScript::init()
{
	m_bInit = false;

	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	//if(Level->GetState() == LEVEL_STATE::STOP)
		//Level->ChangeState(LEVEL_STATE::PLAY);

	TurnBGM();

	// NextLevel 충돌체 끄기
	CGameObject* NextLevel = Level->FindObjectByName(L"ToNextLevel");
	NextLevel->Collider2D()->SetColliderActivate(false);

	// 게임 진행시간 초기화
	m_fTimeLimit = 100.f;
	m_fTimer = m_fTimeLimit;

	// SlowModeCoolDown 초기화
	m_fSlowModeTimer = 11.f;
	m_bSlowActivated = false;

	// RecordManager 기록 시작
	CRecordManager::GetInst()->ResetRecordData();
	CRecordManager::GetInst()->StartRecording();

	// StageNoti 초기화
	m_StageNoti = Level->FindObjectByName(L"StageNoti");
	m_StageNoti->MeshRender()->GetMaterial()->SetTexParam(TEX_0, nullptr);

	// PostProcess 초기화
	m_PostProcess = Level->FindObjectByName(L"PostProcess");
	m_PostProcess->MeshRender()->SetMaterial(nullptr);

	// FadeInOut 초기화
	m_FadeInOut = Level->FindObjectByName(L"FadeInOut");
	int OverlayUse = 1;
	m_FadeInOut->MeshRender()->GetMaterial()->SetScalarParam(INT_0, &OverlayUse);
	float zero = 0.f;
	m_FadeInOut->MeshRender()->GetMaterial()->SetScalarParam(INT_0, &zero);

	// EndWall 찾기
	vector<CGameObject*> Walls = Level->FindLayerByName(L"Wall")->GetParentObject();
	for (size_t i = 0; i < Walls.size(); i++)
	{
		CWallScript* WallScript = Walls[i]->GetScript<CWallScript>();

		if (WallScript->IsEndWall())
		{
			m_EndWall = Walls[i];
		}
	}

	GetCurLevelIdx();
	InitPlayerPos();

	// Player Mtrl 초기화
	CGameObject* Player = Level->FindObjectByName(L"Player");
	int InitTransparent = 0;
	int InitOverlay = 0;
	Vec4 Init = Vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
	Player->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &Init);
	Player->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &InitOverlay);
	Player->MeshRender()->GetMaterial()->SetScalarParam(INT_2, &InitTransparent);

	// 시작시 Distortion
	CGameObject* Dist = new CGameObject;
	Dist->AddComponent(new CTransform);
	Dist->AddComponent(new CMeshRender);
	Dist->MeshRender()->SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Dist->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"NewDistortionMtrl"));
	Dist->Transform()->SetRelativeScale(Vec3(1280.f, 720.f, 0.f));
	Dist->SetLifeSpan(0.3f);
	SpawnGameObject(Dist, Vec3(0.f, 0.f, 0.f), L"UI");

	// Sound Effect
	Ptr<CSound> SFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\LevelStart.wav");
	SFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.3f, false);
}

void CGameManagerScript::FadeIn(CGameObject* _Obj, float& _Opacity, float _MaxBright, float _Speed)
{
	if (_Obj == nullptr || _Obj->MeshRender() == nullptr)
		return;

	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	Vec3 cameraPos = Level->FindObjectByName(L"MainCamera")->Transform()->GetRelativePos();
	Vec3 overlayPos = _Obj->Transform()->GetRelativePos();
	overlayPos.x = cameraPos.x;
	overlayPos.y = cameraPos.y;
	_Obj->Transform()->SetRelativePos(overlayPos);

	float targetOpacity = _MaxBright;
	_Opacity = Lerp(_Opacity, targetOpacity, _Speed * DT);
	_Obj->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &_Opacity);
}

void CGameManagerScript::FadeOut(CGameObject* _Obj, float& _Opacity, float _MaxDark, float _Speed)
{
	if (_Obj == nullptr || _Obj->MeshRender() == nullptr)
		return;

	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	Vec3 cameraPos = Level->FindObjectByName(L"MainCamera")->Transform()->GetRelativePos();
	Vec3 overlayPos = _Obj->Transform()->GetRelativePos();
	overlayPos.x = cameraPos.x;
	overlayPos.y = cameraPos.y;
	_Obj->Transform()->SetRelativePos(overlayPos);

	float targetOpacity = _MaxDark;
	_Opacity = Lerp(_Opacity, targetOpacity, _Speed * DT);
	_Obj->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &_Opacity);
}

void CGameManagerScript::UIFadeIn()
{
	float targetOpacity = 0.f;
	m_fFadeOpacity = Lerp(m_fFadeOpacity, targetOpacity, 20.f * DT);
	m_FadeInOut->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &m_fFadeOpacity);
}

void CGameManagerScript::UIFadeOut()
{
	float targetOpacity = 1.f;
	m_fFadeOpacity = Lerp(m_fFadeOpacity, targetOpacity, 30.f * DT);
	m_FadeInOut->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &m_fFadeOpacity);
}

void CGameManagerScript::GetCurLevelIdx()
{
	CLevel* curLevel = CLevelManager::GetInst()->GetCurLevel();
	if (curLevel->GetName() == L"Level1")
		m_iLevelIdx = 0;
	else if (curLevel->GetName() == L"Level2")
		m_iLevelIdx = 1;
	else if (curLevel->GetName() == L"Level3")
		m_iLevelIdx = 2;
}

bool CGameManagerScript::IsFadeInEnd()
{
	float Opacity;
	m_FadeInOut->MeshRender()->GetMaterial()->GetScalarParam(FLOAT_0, &Opacity);

	return Opacity <= 0.f;
}

bool CGameManagerScript::IsFadeOutEnd()
{
	float Opacity;
	m_FadeInOut->MeshRender()->GetMaterial()->GetScalarParam(FLOAT_0, &Opacity);

	return Opacity >= 0.9f;
}

void CGameManagerScript::TurnBGM()
{
}

void CGameManagerScript::InitPlayerPos()
{
	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* Player = Level->FindObjectByName(L"Player");

	switch (m_iLevelIdx)
	{
	case 0:
		break;
	case 1:
		Player->Transform()->SetRelativePos(Vec3(-655.f, -430.f, 0.f));
		break;
	case 2:
		Player->Transform()->SetRelativePos(Vec3(-620.f, -156.f, 0.f));
		break;
	}

}
