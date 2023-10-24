#include "pch.h"
#include "CTitleScript.h"

#include <Engine\CKeyManager.h>
#include <Engine\CEventManager.h>
#include <Engine\CResourceManager.h>
#include "CLevelSaveLoad.h"

CTitleScript::CTitleScript()
	: CScript((UINT)SCRIPT_TYPE::TITLESCRIPT)
{
}

CTitleScript::~CTitleScript()
{
}


void CTitleScript::begin()
{
	//BGM
	Ptr<CSound> BGM = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\TitleBGM.ogg");
	BGM->Play(0, CTimeManager::GetInst()->GetTimeFactor(), 0.2f, false);
}

void CTitleScript::tick()
{
	if (KEY_PRESSED(KEY::SPACE))
	{
		CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"level\\TestLevel.lv");
		tEvent evn = {};
		evn.Type = EVENT_TYPE::LEVEL_CHANGE;
		evn.wParam = (DWORD_PTR)pLoadedLevel;
		CEventManager::GetInst()->AddEvent(evn);

		Ptr<CSound> BGM = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\TitleBGM.ogg");
		BGM->Stop();
		BGM = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\BGM.ogg");
		BGM->Play(0, CTimeManager::GetInst()->GetTimeFactor(), 0.2f, false);
	}
}
