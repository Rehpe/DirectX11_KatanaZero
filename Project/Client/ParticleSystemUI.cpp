#include "pch.h"
#include "ParticleSystemUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CParticleSystem.h>
#include <Engine\ptr.h>
#include <Engine\CTexture.h>
#include <Engine\CResourceManager.h>

#include "ParamUI.h"

ParticleSystemUI::ParticleSystemUI()
	: ComponentUI("##ParticleSystem", COMPONENT_TYPE::PARTICLESYSTEM)
	,SpawnModule(true)
	,ScaleChangeModule(false)
	,ColorChangeModule(false)
	,AddVelocityModule(false)
	,DragModule(false)
	,NoiseForceModule(false)
	,RenderModule(false)
	,Render_VelocityAlignment(false)
	,Render_VelocityScale(false)
{
	SetName("ParticleSystem");
}

ParticleSystemUI::~ParticleSystemUI()
{
}

void ParticleSystemUI::init()
{
	m_PModule = GetTarget()->ParticleSystem()->GetModule();

	SpawnModule = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN];
	ScaleChangeModule = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE];
	ColorChangeModule = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE];
	AddVelocityModule = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY];
	DragModule = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG];
	NoiseForceModule = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE];
	RenderModule = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER];
	Render_VelocityAlignment = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] && m_PModule.VelocityAlignment;
	Render_VelocityScale = m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] && m_PModule.VelocityScale;
}

int ParticleSystemUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	ImGui::BeginChild("##Particle");

	CheckParticleTex();
	CheckSpawnModule();
	CheckScaleChangeModule();
	CheckColorChangeModule();
	CheckAddVelocityModule();
	CheckDragModule();
	CheckNoiseForceModule();
	CheckRenderModule();

	GetTarget()->ParticleSystem()->SetModule(m_PModule);
	ImGui::EndChild();

		return TRUE;
	}

void ParticleSystemUI::CheckParticleTex()
{
	Ptr<CTexture> ParticleTex = GetTarget()->ParticleSystem()->GetParticleTex();
	
	string ParticleTexKey;
	
	if (ParticleTex == nullptr)
		ParticleTexKey = "NULL";
	else
		ParticleTexKey = string(ParticleTex->GetKey().begin(), ParticleTex->GetKey().end());
		
	//ImTextureID ParticleTexID = (void*)(ParticleTex->GetSRV().Get());
	//ImGui::Image(ParticleTexID, ImVec2(50, 50));
	
	//ImGui::Text(key.c_str());

	ParamUI::Param_Tex(ParticleTexKey.c_str(), ParticleTex, this, (UI_DELEGATE_1)&ParticleSystemUI::SelectTexture);
	ImGui::SameLine();
	if (ImGui::Button(" X "))
	{
		GetTarget()->ParticleSystem()->SetParticleTex(nullptr);
	}
}

void ParticleSystemUI::CheckSpawnModule()
	{
	ImVec2 ContentSize = ImVec2(ImGui::GetWindowWidth() * 0.95f, 270);// ImGui::GetContentRegionAvail().y);
	//스폰 모듈
	if (ImGui::Checkbox("Spawn Module", &SpawnModule))
	{
		m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = SpawnModule;
		GetTarget()->ParticleSystem()->SetModule(m_PModule);
	}
		if (SpawnModule)
		{
			ImGui::BeginChild("##SpawnModule", ContentSize, true);
			ImGui::Text("Particle MaxCount");
			ImGui::SameLine();
			ImGui::DragInt("##MaxCount", &(m_PModule.iMaxParticleCount));

			ImGui::Text("Spawn Rate	     ");
			ImGui::SameLine();
			ImGui::DragInt("##SpawnRate", &(m_PModule.SpawnRate));

			ImGui::Text("Min LifeTime	  ");
			ImGui::SameLine();
			ImGui::DragFloat("##MinLifeTime", &(m_PModule.MinLifeTime));
			ImGui::Text("Max LifeTime	  ");
			ImGui::SameLine();
			ImGui::DragFloat("##MaxLifeTime", &(m_PModule.MaxLifeTime));

			ImGui::Text("Minimum Scale	 ");
			ImGui::SameLine();
			float scaleMin[4] = { m_PModule.vSpawnScaleMin.x, m_PModule.vSpawnScaleMin.y, m_PModule.vSpawnScaleMin.z, 0.0f };
			if (ImGui::DragFloat4("##MinScaleVec", scaleMin)) {
				m_PModule.vSpawnScaleMin.x = scaleMin[0];
				m_PModule.vSpawnScaleMin.y = scaleMin[1];
				m_PModule.vSpawnScaleMin.z = scaleMin[2];
			}
			//ImGui::DragFloat4("##MinScaleVec", &(m_PModule.vSpawnScaleMin.x, m_PModule.vSpawnScaleMin.y, m_PModule.vSpawnScaleMin.z));
			ImGui::Text("Maximum Scale    ");
			ImGui::SameLine();
			float scaleMax[4] = { m_PModule.vSpawnScaleMax.x, m_PModule.vSpawnScaleMax.y, m_PModule.vSpawnScaleMax.z, 0.0f };
			if (ImGui::DragFloat4("##MaxScaleVec", scaleMax)) {
				m_PModule.vSpawnScaleMax.x = scaleMax[0];
				m_PModule.vSpawnScaleMax.y = scaleMax[1];
				m_PModule.vSpawnScaleMax.z = scaleMax[2];
			}
			//ImGui::DragFloat4("##MaxScaleVec", &(m_PModule.vSpawnScaleMax.x, m_PModule.vSpawnScaleMax.y, m_PModule.vSpawnScaleMax.z));

			ImGui::Text("Spawn Color      ");
			ImGui::SameLine();
			static bool alpha_preview = true;
			static bool alpha_half_preview = false;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;
			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
			ImGui::ColorEdit3("##SpawnColor", (float*)&(m_PModule.vSpawnColor), ImGuiColorEditFlags_Float | misc_flags);

			// SpawnShapeType 
			bool Box, Sphere = false;
			if (m_PModule.SpawnShapeType == 0)
			{
				Box = true;
				Sphere = false;
			}
			if (m_PModule.SpawnShapeType == 1)
			{
				Sphere = true;
				Box = false;
			}
			ImGui::Text("Spawn Shape	  ");
			ImGui::SameLine();
			if (ImGui::Checkbox("Box", &Box))
			{
				m_PModule.SpawnShapeType = 0;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Sphere", &Sphere))
			{
				m_PModule.SpawnShapeType = 1;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}
			if (Box)
			{
				ImGui::Text("Box Scale    ");
				ImGui::SameLine();
				float BoxScale[3] = { m_PModule.vBoxShapeScale.x, m_PModule.vBoxShapeScale.y, m_PModule.vBoxShapeScale.z};
				if (ImGui::DragFloat2("##BoxScale", BoxScale)) {
					m_PModule.vBoxShapeScale.x = BoxScale[0];
					m_PModule.vBoxShapeScale.y = BoxScale[1];
					m_PModule.vBoxShapeScale.z = BoxScale[2];
				}
				
			}
			if (Sphere)
			{
				ImGui::Text("Sphere Radius");
				ImGui::SameLine();
				ImGui::DragFloat("##SphereRadius", &(m_PModule.fSphereShapeRadius));
			}

			// Space
			bool World, Local = false;
			if (m_PModule.Space == 0)
			{
				World = true;
				Local = false;
			}
			if (m_PModule.Space == 1)
			{
				Local = true;
				World = false;
			}
			ImGui::Text("Space			  ");
			ImGui::SameLine();
			if (ImGui::Checkbox("World", &World))
			{
				m_PModule.Space = 0;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Local", &Local))
			{
				m_PModule.Space = 1;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}
			ImGui::EndChild();
		}

	}

	void ParticleSystemUI::CheckScaleChangeModule()
	{
		//크기 변경 모듈
		if (ImGui::Checkbox("ScaleChange Module", &ScaleChangeModule))
		{
			m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE] = ScaleChangeModule;
			GetTarget()->ParticleSystem()->SetModule(m_PModule);
		}

		if (ScaleChangeModule)
		{
			ImVec2 ContentSize = ImVec2(ImGui::GetWindowWidth() * 0.95f, 60);
			ImGui::BeginChild("##ScaleChange", ContentSize, true);
			ImGui::Text("Start Scale Ratio");
			ImGui::SameLine();
			ImGui::DragFloat("##StartScaleRatio", &(m_PModule.StartScale));
			ImGui::Text("End Scale Ratio");
			ImGui::SameLine();
			ImGui::DragFloat("##EndScaleRatio", &(m_PModule.EndScale));
			ImGui::EndChild();
		}
	}

	void ParticleSystemUI::CheckColorChangeModule()
	{
		//색상 변경 모듈
		if (ImGui::Checkbox("ColorChange Module", &ColorChangeModule))
		{
			m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE] = ColorChangeModule;
			GetTarget()->ParticleSystem()->SetModule(m_PModule);
		}

		if (ColorChangeModule)
		{
			ImVec2 ContentSize = ImVec2(ImGui::GetWindowWidth() * 0.95f, 60);
			ImGui::BeginChild("##ColorChange", ContentSize, true);
			static bool alpha_preview = true;
			static bool alpha_half_preview = false;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;
			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

			ImGui::Text("Start Color");
			ImGui::SameLine();
			ImGui::ColorEdit3("##StartColor", (float*)&(m_PModule.vStartColor), ImGuiColorEditFlags_Float | misc_flags);
			ImGui::Text("End Color");
			ImGui::SameLine();
			ImGui::ColorEdit3("##EndColor", (float*)&(m_PModule.vEndColor), ImGuiColorEditFlags_Float | misc_flags);
			ImGui::EndChild();
		}
	}

	void ParticleSystemUI::CheckAddVelocityModule()
	{
		//크기 변경 모듈
		if (ImGui::Checkbox("AddVelocity Module", &AddVelocityModule))
		{
			m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = AddVelocityModule;
			GetTarget()->ParticleSystem()->SetModule(m_PModule);
		}

		if (AddVelocityModule)
		{
			ImVec2 ContentSize = ImVec2(ImGui::GetWindowWidth() * 0.95f, 90);
			ImGui::BeginChild("##AddVelocity", ContentSize, true);
			bool FromCenter, ToCenter, Directional = false;
			
			if (m_PModule.AddVelocityType == 0)
			{
				FromCenter = true;
				ToCenter = false;
				Directional = false;
			}
			else if (m_PModule.AddVelocityType == 1)
			{
				FromCenter = false;
				ToCenter = true;
				Directional = false;
			}
			else if (m_PModule.AddVelocityType == 2)
			{
				FromCenter = false;
				ToCenter = false;
				Directional = true;
			}
			
			if (ImGui::Checkbox("From Center", &FromCenter))
			{
				m_PModule.AddVelocityType = 0;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("To Center", &ToCenter))
			{
				m_PModule.AddVelocityType = 1;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Directional", &Directional))
			{
				m_PModule.AddVelocityType = 2;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}

			if (m_PModule.AddVelocityType == 2)
			{
				static float fAngle = 0;
				ImGui::Text("Directional Angle");
				ImGui::SameLine();
				ImGui::DragFloat("##DirAngle", &fAngle, 1.f, 0.f, 360.f);

				float fRadianAngle = fAngle * XM_PI / 180;
				m_PModule.vVelocityDir = Vec4((float)sin(fRadianAngle), (float)cos(fRadianAngle), 0, 0);
			}

			ImGui::Text("Speed");
			ImGui::SameLine();
			ImGui::DragFloat("##Speed", &(m_PModule.Speed), 1.f, 0.f);
			ImGui::EndChild();
		}

	}

	void ParticleSystemUI::CheckDragModule()
	{
		//속도 제한 모듈
		if (ImGui::Checkbox("Drag Module", &DragModule))
		{
			m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG] = DragModule;
			GetTarget()->ParticleSystem()->SetModule(m_PModule);
		}

		if (DragModule)
		{
			ImVec2 ContentSize = ImVec2(ImGui::GetWindowWidth() * 0.95f, 60);
			ImGui::BeginChild("##Drag", ContentSize, true);
			ImGui::Text("StartDrag");
			ImGui::SameLine();
			ImGui::DragFloat("##StartDrag", &(m_PModule.StartDrag));
			ImGui::Text("EndDrag");
			ImGui::SameLine();
			ImGui::DragFloat("##EndDrag", &(m_PModule.EndDrag));
			ImGui::EndChild();
		}
	}

	void ParticleSystemUI::CheckNoiseForceModule()
	{
		//속도 제한 모듈
		if (ImGui::Checkbox("NoiseForce Module", &NoiseForceModule))
		{
			m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = NoiseForceModule;
			GetTarget()->ParticleSystem()->SetModule(m_PModule);
		}

		if (NoiseForceModule)
		{
			ImVec2 ContentSize = ImVec2(ImGui::GetWindowWidth() * 0.95f, 60);
			ImGui::BeginChild("##NoiseForce", ContentSize, true);
			ImGui::Text("NoiseTerm");
			ImGui::SameLine();
			ImGui::DragFloat("##NoiseTerm", &(m_PModule.fNoiseTerm));
			ImGui::Text("NoiseForce");
			ImGui::SameLine();
			ImGui::DragFloat("##NoiseForce", &(m_PModule.fNoiseForce));
			ImGui::EndChild();
		}
	}

	void ParticleSystemUI::CheckRenderModule()
	{
		//렌더 모듈
		if (ImGui::Checkbox("Render Module", &RenderModule))
		{
			m_PModule.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = RenderModule;
			GetTarget()->ParticleSystem()->SetModule(m_PModule);
		}

		if (RenderModule)
		{
			ImVec2 ContentSize = ImVec2(ImGui::GetWindowWidth() * 0.95f, 90);
			ImGui::BeginChild("##Render", ContentSize, true);

			// 속도정렬
			if (ImGui::Checkbox("VelocityAlign", &Render_VelocityAlignment))
			{
				m_PModule.VelocityAlignment = Render_VelocityAlignment;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}
			ImGui::SameLine();
			// 속도비례크기
			if (ImGui::Checkbox("VelocityScale", &Render_VelocityScale))
			{ 
				m_PModule.VelocityScale = Render_VelocityScale;
				GetTarget()->ParticleSystem()->SetModule(m_PModule);
			}

			ImGui::Text("MaxSpeed");
			ImGui::SameLine();
			ImGui::DragFloat("##MaxSpeed", &(m_PModule.vMaxSpeed));
			ImGui::Text("MaxVelocityScale");
			ImGui::SameLine();
			ImGui::DragFloat2("##MaxVelocityScale", &(m_PModule.vMaxVelocityScale.x, m_PModule.vMaxVelocityScale.y));
			ImGui::EndChild();
		}
	}

	void ParticleSystemUI::SelectTexture(DWORD_PTR _Key)
	{
		string strKey = (char*)_Key;
		Ptr<CTexture> pTex = CResourceManager::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));

		GetTarget()->ParticleSystem()->SetParticleTex(pTex);
	}
	
