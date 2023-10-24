#include "pch.h"
#include "MenuUI.h"

#include <Engine\CEventManager.h>
#include <Engine\CGameObject.h>
#include <Engine\component.h>
#include <Engine\CScript.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CCollisionManager.h>

#include <Script\CScriptManager.h>
#include <Script\CLevelSaveLoad.h>

#include "ImGuiManager.h"
#include "OutlinerUI.h"
#include "InspectorUI.h"
#include "ListUI.h"

MenuUI::MenuUI()
	: UI("##Menu")
{
	SetName("Menu");
}

MenuUI::~MenuUI()
{
}

void MenuUI::finaltick()
{
	if (!IsActive())
		return;

	// 따로 뜨는 창이 아닌 게임 윈도우에 그대로 붙이고 싶은 것이므로
	// 부모의 상속으로 render을 돌리지 않고 finaltick에서 본인의 render을 호출하는 식으로 설계
	render_update();
}

int MenuUI::render_update()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Level"))
			{
				// Level 저장
				CLevelSaveLoad::SaveLevel(L"level\\TestLevel.lv", CLevelManager::GetInst()->GetCurLevel());
				//CLevelSaveLoad::SaveLevel(L"level\\TutoLevel2.lv", CLevelManager::GetInst()->GetCurLevel());
			}

			if (ImGui::MenuItem("Load Level"))
			{
				// Level 불러오기
				CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"level\\TestLevel.lv");

				tEvent evn = {};
				evn.Type = EVENT_TYPE::LEVEL_CHANGE;
				evn.wParam = (DWORD_PTR)pLoadedLevel;

				CEventManager::GetInst()->AddEvent(evn);

				// InspectorUI에 nullptr 밀어넣기.
				InspectorUI* Inspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");
				Inspector->SetTargetObject(nullptr);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			// 현재 레벨에 게임오브젝트 생성
			if (ImGui::MenuItem("Create Empty Object"))
			{
				CreateEmptyObject();
			}
			ImGui::Separator();


			if (ImGui::BeginMenu("Add Component"))
			{
				for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
				{
					if (ImGui::MenuItem(ToString((COMPONENT_TYPE)i)))
					{
						AddComponent(COMPONENT_TYPE(i));
					}
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Add Script"))
			{
				// 스크립트 매니저 내의 모든 스크립트를 벡터로 가져온다
				vector<wstring> vecScripts;
				CScriptManager::GetScriptInfo(vecScripts);

				// 스크립트 개수만큼 반복하여 스크립트 이름 출력, 선택 시 해당 스크립트 Add함수 호출
				for (size_t i = 0; i < vecScripts.size(); ++i)
				{
					string strScriptName = string(vecScripts[i].begin(), vecScripts[i].end());
					if (ImGui::MenuItem(strScriptName.c_str()))
					{
						AddScript(vecScripts[i]);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Add Prefab"))
			{
				AddPrefab();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Delete Object"))
			{
				DeleteSelectedObj();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Level"))
		{
			CLevel* CurLevel = CLevelManager::GetInst()->GetCurLevel();
			bool PlayEnable = true;
			bool PauseEnable = true;
			bool StopEnable = true;

			if (CurLevel->GetState() == LEVEL_STATE::PLAY)
			{
				PlayEnable = false;
				PauseEnable = true;
				StopEnable = true;
			}
			else if (CurLevel->GetState() == LEVEL_STATE::PAUSE)
			{
				PlayEnable = true;
				PauseEnable = false;
				StopEnable = true;
			}
			else if (CurLevel->GetState() == LEVEL_STATE::STOP)
			{
				PlayEnable = true;
				PauseEnable = false;
				StopEnable = false;
			}

			if (ImGui::MenuItem("Play", nullptr, nullptr, PlayEnable))
			{
				// Level Play 시 Play 직전의 레벨을 임시로 저장한다.
				CLevelSaveLoad::SaveLevel(L"level\\Temp.lv", CurLevel);
				CurLevel->ChangeState(LEVEL_STATE::PLAY);
			}
			else if (ImGui::MenuItem("Pause", nullptr, nullptr, PauseEnable))
			{
				CurLevel->ChangeState(LEVEL_STATE::PAUSE);
			}
			else if (ImGui::MenuItem("Stop", nullptr, nullptr, StopEnable))
			{
				// Stop모드일 시 Play레벨에 저장했던 임시 레벨을 불러온다.
				CurLevel->ChangeState(LEVEL_STATE::STOP);
				CLevel* pNewLevel = CLevelSaveLoad::LoadLevel(L"level\\Temp.lv");

				tEvent evn = {};
				evn.Type = EVENT_TYPE::LEVEL_CHANGE;
				evn.wParam = DWORD_PTR(pNewLevel);
				CEventManager::GetInst()->AddEvent(evn);

				// InspectorUI에 nullptr 밀어넣기.
				InspectorUI* Inspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");
				Inspector->SetTargetObject(nullptr);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Collision Setting"))
			{
				m_bOpenCollisionSettingPopup = true;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Level Name"))
			{
				
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Layer"))
		{
			if (ImGui::MenuItem("Layer Setting"))
			{
				m_bOpenLayerSettingPopup = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("New Mtrl"))
			{
				m_bOpenCreateMtrlPopup = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_bOpenCreateMtrlPopup)
	{
		ImGui::OpenPopup("Create New Mtrl");
	}


	if (ImGui::BeginPopup("Create New Mtrl"))
	{
		ImGui::Text("Material");
		ImGui::SameLine();

		// Mtrl 이름
		char input_text_buffer[256] = { 0 };
		strcpy_s(input_text_buffer, m_sMtrlName.c_str());
		if(ImGui::InputText("##MtrlUIName", input_text_buffer, sizeof(input_text_buffer)))
			m_sMtrlName = input_text_buffer;

		// GraphicsShader 이름
		ImGui::Text("Shader  ");
		ImGui::SameLine();

		const map<wstring, Ptr<CResource>>& mapShader = CResourceManager::GetInst()->GetResources(RES_TYPE::GRAPHICS_SHADER);

		vector<string> vecShader = { "SelectShader" };
		for (const auto& pair : mapShader)
		{
			vecShader.push_back(string(pair.first.begin(), pair.first.end()));
		}
		static int SelectedShader = 0;

		if (ImGui::BeginCombo("##Select Shader", vecShader[SelectedShader].c_str()))
		{
			for (int i = 0; i < vecShader.size(); ++i) {
				bool isSelected = (SelectedShader == i);

				// Add the item to the combo box and check if it's selected
				if (ImGui::Selectable(vecShader[i].c_str(), isSelected)) {
					SelectedShader = i; // Update the index of the selected item
					m_sShaderKey = vecShader[i];
				}

				// Set the highlighted item as the selected item
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
		
		if (ImGui::Button("Save New Mtrl"))
		{
			if (SelectedShader == 0)
			{
				wchar_t szStr[256] = {};
				wsprintf(szStr, L"Shader를 지정해주세요");
				MessageBox(nullptr, szStr, L"Material 저장 실패", MB_OK);
				return 0;
			}
			else
			{
				Ptr<CMaterial> NewMtrl = new CMaterial(false);
				
				wstring ShaderKey = stringToWstring(m_sShaderKey);
				Ptr<CGraphicsShader> pShader = CResourceManager::GetInst()->FindRes<CGraphicsShader>(ShaderKey);
			
				wstring MtrlName = stringToWstring(m_sMtrlName);
				NewMtrl->SetName(MtrlName);
				NewMtrl->SetShader(pShader);

				wstring MtrlKey = MtrlName + L".mtrl";
				NewMtrl->Save(MtrlKey);

				m_sMtrlName = "";
				SelectedShader = 0;
				m_bOpenCreateMtrlPopup = false;
			}
		}
		ImGui::EndPopup();
	}

	if (m_bOpenCollisionSettingPopup)
	{
		ImGui::OpenPopup("Collision Setting");
	}

	if (ImGui::BeginPopup("Collision Setting"))
	{
		CollisionSetting();
		ImGui::EndPopup();
	}

	if (m_bOpenLayerSettingPopup)
	{
		ImGui::OpenPopup("Setting Layer");
	}


	if (ImGui::BeginPopup("Setting Layer"))
	{
		LayerSetting();
		ImGui::EndPopup();
	}

	return 0;
}

void MenuUI::CreateEmptyObject()
{
	CGameObject* pNewObject = new CGameObject;

	// 생성된 모든 오브젝트에는 Transform 컴포넌트가 기본적으로 붙어있다
	pNewObject->AddComponent(new CTransform);
	pNewObject->SetName(L"New Object");

	SpawnGameObject(pNewObject, Vec3(0.f, 0.f, 0.f), L"Default");

	// Outliner 를 가져와, 새로 추가된 오브젝트를 선택상태로 지정한다
	OutlinerUI* outliner = (OutlinerUI*)ImGuiManager::GetInst()->FindUI("##Outliner");
	outliner->SetSelectedNodeData(DWORD_PTR(pNewObject));
}

void MenuUI::AddComponent(COMPONENT_TYPE _type)
{
	// Outliner 와 Inspector 를 가져온다.
	OutlinerUI* outliner = (OutlinerUI*)ImGuiManager::GetInst()->FindUI("##Outliner");
	InspectorUI* inspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");

	// 선택된 오브젝트를 가져와서 ComponentType 에 맞는 컴포넌트를 생성해서 추가한다.
	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	// 선택된 오브젝트가 없다면 아무 일도 일어나지 않음
	if (nullptr == pSelectedObject)
		return;

	switch (_type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		pSelectedObject->AddComponent(new CTransform);
		break;
	case COMPONENT_TYPE::COLLIDER2D:
		pSelectedObject->AddComponent(new CCollider2D);
		break;
	case COMPONENT_TYPE::COLLIDER3D:
		//pSelectedObject->AddComponent(new CCollider3D);
		break;
	case COMPONENT_TYPE::ANIMATOR2D:
		pSelectedObject->AddComponent(new CAnimator2D);
		break;
	case COMPONENT_TYPE::ANIMATOR3D:
		//pSelectedObject->AddComponent(new CAnimator3D);
		break;
	case COMPONENT_TYPE::LIGHT2D:
		pSelectedObject->AddComponent(new CLight2D);
		break;
	case COMPONENT_TYPE::LIGHT3D:
		//pSelectedObject->AddComponent(new CLight3D);
		break;
	case COMPONENT_TYPE::CAMERA:
		pSelectedObject->AddComponent(new CCamera);
		break;
	case COMPONENT_TYPE::RIGIDBODY2D:
		pSelectedObject->AddComponent(new CRigidBody2D);
		break;
	case COMPONENT_TYPE::FSM_AI:
		pSelectedObject->AddComponent(new CFSM);
		break;
	case COMPONENT_TYPE::MESHRENDER:
		pSelectedObject->AddComponent(new CMeshRender);
		break;
	case COMPONENT_TYPE::PARTICLESYSTEM:
		pSelectedObject->AddComponent(new CParticleSystem);
		break;
	case COMPONENT_TYPE::TILEMAP:
		pSelectedObject->AddComponent(new CTileMap);
		break;
	case COMPONENT_TYPE::LANDSCAPE:
		//pSelectedObject->AddComponent(new CLandScape);
		break;
	case COMPONENT_TYPE::DECAL:
		//pSelectedObject->AddComponent(new CDecal);
		break;
	}

	// Inspector 에 새롭게 추가된 컴포넌트를 알리기 위해서 타겟을 다시 알려준다.
	inspector->SetTargetObject(pSelectedObject);
}

void MenuUI::AddScript(const wstring& _strScriptName)
{
	// Outliner 와 Inspector 를 가져온다.
	OutlinerUI* Outliner = (OutlinerUI*)ImGuiManager::GetInst()->FindUI("##Outliner");
	InspectorUI* Inspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");

	// 선택된 오브젝트를 가져와서 Script를 추가한다.
	CGameObject* pSelectedObject = Outliner->GetSelectedObject();

	if (nullptr == pSelectedObject)
		return;

	CScript* pScript = CScriptManager::GetScript(_strScriptName);
	pSelectedObject->AddComponent(pScript);

	// Inspector 새로고침
	Inspector->SetTargetObject(pSelectedObject);
}

void MenuUI::AddPrefab()
{
	// Outliner 와 Inspector 를 가져온다.
	OutlinerUI* Outliner = (OutlinerUI*)ImGuiManager::GetInst()->FindUI("##Outliner");
	InspectorUI* Inspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");

	// 선택된 오브젝트를 가져와서 Prefab에 추가한다.
	CGameObject* pSelectedObject = Outliner->GetSelectedObject();

	if (nullptr == pSelectedObject)
		return;

	pSelectedObject->ChangeLayer(-1);

	wstring PrefabFileName = pSelectedObject->GetName() + L".pref";
	CLevelSaveLoad::SavePrefab(PrefabFileName, pSelectedObject);

	// Inspector 새로고침
	Inspector->SetTargetObject(nullptr);
}

void MenuUI::DeleteSelectedObj()
{
	// Outliner 와 Inspector 를 가져온다.
	OutlinerUI* outliner = (OutlinerUI*)ImGuiManager::GetInst()->FindUI("##Outliner");
	InspectorUI* inspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");

	// 선택된 오브젝트를 가져온다.
	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	// EventManager를 통해 해당 오브젝트를 삭제한다.
	tEvent evn = {};
	evn.Type = EVENT_TYPE::DELETE_OBJECT;
	evn.wParam = (DWORD_PTR)pSelectedObject;

	CEventManager::GetInst()->AddEvent(evn);

	// Inspector 새로고침
	inspector->SetTargetObject(nullptr);
}

void MenuUI::SelectShader(DWORD_PTR _key)
{
	// 인자로 들어온 키값으로 리소스매니저의 맵에서 해당 Material를 찾아 들고 있는다
	string strKey = (char*)_key;
	Ptr<CGraphicsShader> pMaterial = CResourceManager::GetInst()->FindRes<CGraphicsShader>(wstring(strKey.begin(), strKey.end()));
	m_sShaderKey = strKey;
}

void MenuUI::CollisionSetting()
{
	CLevel* CurLevel = CLevelManager::GetInst()->GetCurLevel();

	vector<wstring> layerNames;
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = CurLevel->GetLayer(i);
		if (pLayer)
		{
			layerNames.push_back(pLayer->GetName());
		}
		else
		{
			layerNames.push_back(L"");
		}
	}

	for (UINT row = 0; row < layerNames.size(); ++row)
	{
		for (UINT col = row + 1; col < layerNames.size(); ++col)
		{
			if (layerNames[row].empty() || layerNames[col].empty())
			{
				continue; // Skip non-existing layers
			}

			string rowName(layerNames[row].begin(), layerNames[row].end());
			string colName(layerNames[col].begin(), layerNames[col].end());

			string label = to_string(row) + " (" + rowName + ") - " + to_string(col) + " (" + colName + ")";
			bool isChecked = CCollisionManager::GetInst()->IsLayerCollisionEnabled(row, col);

			if (ImGui::Checkbox(label.c_str(), &isChecked))
			{
				if (isChecked)
				{
					CCollisionManager::GetInst()->LayerCheck(row, col);
				}
				else
				{
					CCollisionManager::GetInst()->DisableLayerCollision(row, col);
				}
			}
		}
	}

	if (KEY_TAP(KEY::ESC))
	{
		m_bOpenCollisionSettingPopup = false;
	}
}

void MenuUI::LayerSetting()
{
	CLevel* CurLevel = CLevelManager::GetInst()->GetCurLevel();
	
	if (CurLevel == nullptr)
		return;

	//레벨 이름
	wstring wLevelname = CurLevel->GetName();
	char szBuff[50] = {};
	WstringtoString(wLevelname, szBuff, 50);
	string name = szBuff;

	char input_text_buffer[256] = { 0 };
	strcpy_s(input_text_buffer, name.c_str());

	string inputTextId = "##LevelName";
	if (ImGui::InputText(inputTextId.c_str(), input_text_buffer, sizeof(input_text_buffer)))
	{
		string newstrName(input_text_buffer);
		wstring newName = stringToWstring(newstrName);
		CurLevel->SetName(newName);
	}

	for (int i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* Layer = CurLevel->GetLayer(i);
		wstring wname = Layer->GetName();
		char szBuff[50] = {};
		WstringtoString(wname, szBuff, 50);
		string name = szBuff;

		char input_text_buffer[256] = { 0 };
		strcpy_s(input_text_buffer, name.c_str());

		ImGui::Text("Layer %d:", i);
		ImGui::SameLine();

		string inputTextId = "##LayerName" + to_string(i);
		if (ImGui::InputText(inputTextId.c_str(), input_text_buffer, sizeof(input_text_buffer)))
		{
			string newstrName(input_text_buffer);
			wstring newName = stringToWstring(newstrName);
			CurLevel->GetLayer(i)->SetName(newName);
		}
	}

	if (KEY_TAP(KEY::ESC))
	{
		m_bOpenLayerSettingPopup = false;
	}
}
