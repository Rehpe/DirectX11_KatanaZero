#include "pch.h"
#include "CScriptManager.h"

#include "CAttackSpriteScript.h"
#include "CBreakableFloorScript.h"
#include "CCameraMoveScript.h"
#include "CCameraScript.h"
#include "CCursorScript.h"
#include "CDoorScript.h"
#include "CEffectScript.h"
#include "CEnemyDetectorScript.h"
#include "CEnemyGangsterScript.h"
#include "CEnemyGruntScript.h"
#include "CEnemyScript.h"
#include "CFanScript.h"
#include "CGameManagerScript.h"
#include "CGangsterBulletScript.h"
#include "CGroundScript.h"
#include "CGruntAttackSpriteScript.h"
#include "CHudScript.h"
#include "CLaserScript.h"
#include "CLaserSwitchScript.h"
#include "CLightFlickerScript.h"
#include "CMissileScript.h"
#include "CMonsterScript.h"
#include "COneWayPlatformScript.h"
#include "CPlayerScript.h"
#include "CRampScript.h"
#include "CTitleScript.h"
#include "CWallScript.h"

void CScriptManager::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CAttackSpriteScript");
	_vec.push_back(L"CBreakableFloorScript");
	_vec.push_back(L"CCameraMoveScript");
	_vec.push_back(L"CCameraScript");
	_vec.push_back(L"CCursorScript");
	_vec.push_back(L"CDoorScript");
	_vec.push_back(L"CEffectScript");
	_vec.push_back(L"CEnemyDetectorScript");
	_vec.push_back(L"CEnemyGangsterScript");
	_vec.push_back(L"CEnemyGruntScript");
	_vec.push_back(L"CEnemyScript");
	_vec.push_back(L"CFanScript");
	_vec.push_back(L"CGameManagerScript");
	_vec.push_back(L"CGangsterBulletScript");
	_vec.push_back(L"CGroundScript");
	_vec.push_back(L"CGruntAttackSpriteScript");
	_vec.push_back(L"CHudScript");
	_vec.push_back(L"CLaserScript");
	_vec.push_back(L"CLaserSwitchScript");
	_vec.push_back(L"CLightFlickerScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"COneWayPlatformScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CRampScript");
	_vec.push_back(L"CTitleScript");
	_vec.push_back(L"CWallScript");
}

CScript * CScriptManager::GetScript(const wstring& _strScriptName)
{
	if (L"CAttackSpriteScript" == _strScriptName)
		return new CAttackSpriteScript;
	if (L"CBreakableFloorScript" == _strScriptName)
		return new CBreakableFloorScript;
	if (L"CCameraMoveScript" == _strScriptName)
		return new CCameraMoveScript;
	if (L"CCameraScript" == _strScriptName)
		return new CCameraScript;
	if (L"CCursorScript" == _strScriptName)
		return new CCursorScript;
	if (L"CDoorScript" == _strScriptName)
		return new CDoorScript;
	if (L"CEffectScript" == _strScriptName)
		return new CEffectScript;
	if (L"CEnemyDetectorScript" == _strScriptName)
		return new CEnemyDetectorScript;
	if (L"CEnemyGangsterScript" == _strScriptName)
		return new CEnemyGangsterScript;
	if (L"CEnemyGruntScript" == _strScriptName)
		return new CEnemyGruntScript;
	if (L"CEnemyScript" == _strScriptName)
		return new CEnemyScript;
	if (L"CFanScript" == _strScriptName)
		return new CFanScript;
	if (L"CGameManagerScript" == _strScriptName)
		return new CGameManagerScript;
	if (L"CGangsterBulletScript" == _strScriptName)
		return new CGangsterBulletScript;
	if (L"CGroundScript" == _strScriptName)
		return new CGroundScript;
	if (L"CGruntAttackSpriteScript" == _strScriptName)
		return new CGruntAttackSpriteScript;
	if (L"CHudScript" == _strScriptName)
		return new CHudScript;
	if (L"CLaserScript" == _strScriptName)
		return new CLaserScript;
	if (L"CLaserSwitchScript" == _strScriptName)
		return new CLaserSwitchScript;
	if (L"CLightFlickerScript" == _strScriptName)
		return new CLightFlickerScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"COneWayPlatformScript" == _strScriptName)
		return new COneWayPlatformScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CRampScript" == _strScriptName)
 		return new CRampScript;
	if (L"CTitleScript" == _strScriptName)
		return new CTitleScript;
	if (L"CWallScript" == _strScriptName)
		return new CWallScript;
	return nullptr;
}

CScript * CScriptManager::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::ATTACKSPRITESCRIPT:
		return new CAttackSpriteScript;
		break;
	case (UINT)SCRIPT_TYPE::BREAKABLEFLOORSCRIPT:
		return new CBreakableFloorScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CCameraMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERASCRIPT:
		return new CCameraScript;
		break;
	case (UINT)SCRIPT_TYPE::CURSORSCRIPT:
		return new CCursorScript;
		break;
	case (UINT)SCRIPT_TYPE::DOORSCRIPT:
		return new CDoorScript;
		break;
	case (UINT)SCRIPT_TYPE::EFFECTSCRIPT:
		return new CEffectScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYDETECTORSCRIPT:
		return new CEnemyDetectorScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYGANGSTERSCRIPT:
		return new CEnemyGangsterScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYGRUNTSCRIPT:
		return new CEnemyGruntScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYSCRIPT:
		return new CEnemyScript;
		break;
	case (UINT)SCRIPT_TYPE::FANSCRIPT:
		return new CFanScript;
		break;
	case (UINT)SCRIPT_TYPE::GAMEMANAGERSCRIPT:
		return new CGameManagerScript;
		break;
	case (UINT)SCRIPT_TYPE::GANGSTERBULLETSCRIPT:
		return new CGangsterBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::GROUNDSCRIPT:
		return new CGroundScript;
		break;
	case (UINT)SCRIPT_TYPE::GRUNTATTACKSPRITESCRIPT:
		return new CGruntAttackSpriteScript;
		break;
	case (UINT)SCRIPT_TYPE::HUDSCRIPT:
		return new CHudScript;
		break;
	case (UINT)SCRIPT_TYPE::LASERSCRIPT:
		return new CLaserScript;
		break;
	case (UINT)SCRIPT_TYPE::LASERSWITCHSCRIPT:
		return new CLaserSwitchScript;
		break;
	case (UINT)SCRIPT_TYPE::LIGHTFLICKERSCRIPT:
		return new CLightFlickerScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
		break;
	case (UINT)SCRIPT_TYPE::ONEWAYPLATFORMSCRIPT:
		return new COneWayPlatformScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::RAMPSCRIPT:
		return new CRampScript;
		break;
	case (UINT)SCRIPT_TYPE::TITLESCRIPT:
		return new CTitleScript;
		break;
	case (UINT)SCRIPT_TYPE::WALLSCRIPT:
		return new CWallScript;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptManager::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::ATTACKSPRITESCRIPT:
		return L"CAttackSpriteScript";
		break;

	case SCRIPT_TYPE::BREAKABLEFLOORSCRIPT:
		return L"CBreakableFloorScript";
		break;

	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CCameraMoveScript";
		break;

	case SCRIPT_TYPE::CAMERASCRIPT:
		return L"CCameraScript";
		break;

	case SCRIPT_TYPE::CURSORSCRIPT:
		return L"CCursorScript";
		break;

	case SCRIPT_TYPE::DOORSCRIPT:
		return L"CDoorScript";
		break;

	case SCRIPT_TYPE::EFFECTSCRIPT:
		return L"CEffectScript";
		break;

	case SCRIPT_TYPE::ENEMYDETECTORSCRIPT:
		return L"CEnemyDetectorScript";
		break;

	case SCRIPT_TYPE::ENEMYGANGSTERSCRIPT:
		return L"CEnemyGangsterScript";
		break;

	case SCRIPT_TYPE::ENEMYGRUNTSCRIPT:
		return L"CEnemyGruntScript";
		break;

	case SCRIPT_TYPE::ENEMYSCRIPT:
		return L"CEnemyScript";
		break;

	case SCRIPT_TYPE::FANSCRIPT:
		return L"CFanScript";
		break;

	case SCRIPT_TYPE::GAMEMANAGERSCRIPT:
		return L"CGameManagerScript";
		break;

	case SCRIPT_TYPE::GANGSTERBULLETSCRIPT:
		return L"CGangsterBulletScript";
		break;

	case SCRIPT_TYPE::GROUNDSCRIPT:
		return L"CGroundScript";
		break;

	case SCRIPT_TYPE::GRUNTATTACKSPRITESCRIPT:
		return L"CGruntAttackSpriteScript";
		break;

	case SCRIPT_TYPE::HUDSCRIPT:
		return L"CHudScript";
		break;

	case SCRIPT_TYPE::LASERSCRIPT:
		return L"CLaserScript";
		break;

	case SCRIPT_TYPE::LASERSWITCHSCRIPT:
		return L"CLaserSwitchScript";
		break;

	case SCRIPT_TYPE::LIGHTFLICKERSCRIPT:
		return L"CLightFlickerScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
		break;

	case SCRIPT_TYPE::ONEWAYPLATFORMSCRIPT:
		return L"COneWayPlatformScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::RAMPSCRIPT:
		return L"CRampScript";
		break;

	case SCRIPT_TYPE::TITLESCRIPT:
		return L"CTitleScript";
		break;

	case SCRIPT_TYPE::WALLSCRIPT:
		return L"CWallScript";
		break;

	}
	return nullptr;
}