#include "pch.h"
#include "InspectorUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine\CLevelManager.h>

#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "Collider2DUI.h"
#include "CameraUI.h"
#include "Animator2DUI.h"
#include "TileMapUI.h"
#include "Light2DUI.h"
#include "ParticleSystemUI.h"
#include "RigidBody2DUI.h"
#include "FSMUI.h"

#include "LayerUI.h"

#include "MeshUI.h"
#include "TextureUI.h"
#include "MeshDataUI.h"
#include "SoundUI.h"
#include "PrefabUI.h"
#include "GraphicsShaderUI.h"
#include "ComputeShaderUI.h"
#include "MaterialUI.h"

#include "ScriptUI.h"

InspectorUI::InspectorUI()
    : UI("##Inspector")       // UI는 무조건 생성자에서 이름(ID)을 받아간다
    , m_pTargetObj(nullptr)
    , m_arrComUI{}
	, m_arrResUI{}
{
	SetName("Inspector");

	// LayerUI
	m_LayerUI = new LayerUI;
	m_LayerUI->SetSize(0.f, 100.f);
	AddChildUI(m_LayerUI);

	// m_arrComUI에는 Component UI 타입이므로
	// Component UI 생성과 동시에 해당 UI의 반환타입을 각각 지정해준다.
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM] = new TransformUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM]->SetSize(0.f, 120.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::TRANSFORM]);

	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER] = new MeshRenderUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]->SetSize(0.f, 120.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::MESHRENDER]);

	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA] = new CameraUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA]->SetSize(0.f, 150.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::CAMERA]);

	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D] = new Collider2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D]->SetSize(0.f, 150.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::COLLIDER2D]);

	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D] = new Animator2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D]->SetSize(0.f, 150.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::ANIMATOR2D]);

	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D] = new Light2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D]->SetSize(0.f, 150.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::LIGHT2D]);

	m_arrComUI[(UINT)COMPONENT_TYPE::RIGIDBODY2D] = new RigidBody2DUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::RIGIDBODY2D]->SetSize(0.f, 150.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::RIGIDBODY2D]);

	m_arrComUI[(UINT)COMPONENT_TYPE::FSM_AI] = new FSMUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::FSM_AI]->SetSize(0.f, 50.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::FSM_AI]);

	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP] = new TileMapUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP]->SetSize(0.f, 150.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::TILEMAP]);

	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM] = new ParticleSystemUI;
	m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM]->SetSize(0.f, 550.f);
	AddChildUI(m_arrComUI[(UINT)COMPONENT_TYPE::PARTICLESYSTEM]);

	// ResourceUI
	m_arrResUI[(UINT)RES_TYPE::MESHDATA] = new MeshDataUI;
	m_arrResUI[(UINT)RES_TYPE::MESHDATA]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::MESHDATA]);

	m_arrResUI[(UINT)RES_TYPE::MESH] = new MeshUI;
	m_arrResUI[(UINT)RES_TYPE::MESH]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::MESH]);

	m_arrResUI[(UINT)RES_TYPE::TEXTURE] = new TextureUI;
	m_arrResUI[(UINT)RES_TYPE::TEXTURE]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::TEXTURE]);

	m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER] = new GraphicsShaderUI;
	m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::GRAPHICS_SHADER]);

	m_arrResUI[(UINT)RES_TYPE::COMPUTE_SHADER] = new ComputeShaderUI;
	m_arrResUI[(UINT)RES_TYPE::COMPUTE_SHADER]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::COMPUTE_SHADER]);

	m_arrResUI[(UINT)RES_TYPE::PREFAB] = new PrefabUI;
	m_arrResUI[(UINT)RES_TYPE::PREFAB]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::PREFAB]);

	m_arrResUI[(UINT)RES_TYPE::MATERIAL] = new MaterialUI;
	m_arrResUI[(UINT)RES_TYPE::MATERIAL]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::MATERIAL]);

	m_arrResUI[(UINT)RES_TYPE::SOUND] = new SoundUI;
	m_arrResUI[(UINT)RES_TYPE::SOUND]->SetSize(0.f, 0.f);
	AddChildUI(m_arrResUI[(UINT)RES_TYPE::SOUND]);

	// Script의 경우 벡터이므로 생성자에서 미리 생성할 필요가 없다
}

InspectorUI::~InspectorUI()
{
}

void InspectorUI::init()
{
	SetTargetObject(nullptr);
}

void InspectorUI::tick()
{
	
}

int InspectorUI::render_update()
{
	return TRUE;
}

void InspectorUI::SetTargetObject(CGameObject* _Target)
{
	ClearTargetResource();

	// 타겟오브젝트 정보 노출
	m_pTargetObj = _Target;
	m_LayerUI->SetTargetObj(_Target);
	m_LayerUI->SetActive(true);

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == m_arrComUI[i])
			continue;

		m_arrComUI[i]->SetTarget(m_pTargetObj);
	}

	// 타겟 오브젝트가 지정되지 않았다면, 스크립트UI를 전부 비활성화 시킨다.
	if (nullptr == m_pTargetObj)
	{
		for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
		{
			m_vecScriptUI[i]->SetActive(false);
		}
		return;
	}

	// 오브젝트의 스크립트 목록을 받아온다.
	const vector<CScript*>& vecScript = m_pTargetObj->GetScripts();

	// 스크립트UI의 개수가 타겟 오브젝트의 스크립트 수보다 적으면 그만큼 추가해준다.
	if (m_vecScriptUI.size() < vecScript.size())
	{
		UINT iDiffer = UINT(vecScript.size() - m_vecScriptUI.size());
		for (UINT i = 0; i < iDiffer; ++i)
		{
			// 새 스크립트 생성 후, 스크립트UI 벡터에 넣고 + 인스펙터의 자식으로 추가 + 활성화
			ScriptUI* UI = new ScriptUI;
			m_vecScriptUI.push_back(UI);
			AddChildUI(UI);
			UI->SetActive(true);
		}
	}

	// ScriptUI 개수만큼 반복문 돌면서 오브젝트의 스크립트 수만큼만 활성화 시킨다.
	// ( 오브젝트의 스크립트 수보다 ScriptUI가 더 많이 활성화되어있다면, 초과한 만큼 ScriptUI를 비활성화)
	for (size_t i = 0; i < m_vecScriptUI.size(); ++i)
	{
		if (vecScript.size() <= i)
		{
			m_vecScriptUI[i]->SetActive(false);
			continue;
		}

		// 스크립트UI 에게 어떤 스크립트를 담당할지 지정해주고 활성화시킨다.
		m_vecScriptUI[i]->SetTarget(m_pTargetObj);
		m_vecScriptUI[i]->SetScript(vecScript[i]);
		m_vecScriptUI[i]->SetActive(true);
	}
}

void InspectorUI::SetTargetResource(Ptr<CResource> _Res)
{
	ClearTargetObject();

	for (UINT i = 0; i < UINT(RES_TYPE::END); ++i)
	{
		// 인자로 들어온 Resource타입을 제외한 모든 ResourceUI는 꺼준다 
		if (nullptr != m_arrResUI[i])
			m_arrResUI[i]->SetActive(false);
	}

	m_pTargetRes = _Res;

	if (nullptr == m_pTargetRes)
		return;

	RES_TYPE type = _Res->GetType();

	m_arrResUI[(UINT)type]->SetActive(true);
	m_arrResUI[(UINT)type]->SetTargetRes(_Res);
}

void InspectorUI::ClearTargetObject()
{
	m_pTargetObj = nullptr;
	m_LayerUI->SetTargetObj(nullptr);
	m_LayerUI->SetActive(false);

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == m_arrComUI[i])
			continue;

		m_arrComUI[i]->SetTarget(nullptr);
	}
}

void InspectorUI::ClearTargetResource()
{
	m_pTargetRes = nullptr;

	for (UINT i = 0; i < UINT(RES_TYPE::END); ++i)
	{
		if (nullptr != m_arrResUI[i])
		{
			m_arrResUI[i]->SetTargetRes(nullptr);
			m_arrResUI[i]->SetActive(false);
		}
	}
}
