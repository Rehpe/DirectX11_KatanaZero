#pragma once
#include "UI.h"
class MenuUI :
    public UI
{
public:
    MenuUI();
    ~MenuUI();

private:
    // ======== CreateMtrl ========
    bool            m_bOpenCreateMtrlPopup;
    bool            m_bOpenCollisionSettingPopup;
    bool            m_bOpenLayerSettingPopup;
    string          m_sMtrlName;
    string          m_sShaderKey;

public:
    virtual void finaltick() override;
    virtual int render_update() override;

private:
    void CreateEmptyObject();

    // 현재 아웃라이너UI에서 선택된 게임오브젝트에 Component / Script 추가
    void AddComponent(COMPONENT_TYPE _type);        
    void AddScript(const wstring& _strScriptName);  
    void AddPrefab();
    
    void DeleteSelectedObj();

    // CreateMtrl 전용 함수
    void SelectShader(DWORD_PTR _key);

    void CollisionSetting();

    void LayerSetting();
};

