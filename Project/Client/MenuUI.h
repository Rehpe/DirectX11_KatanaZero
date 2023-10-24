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

    // ���� �ƿ����̳�UI���� ���õ� ���ӿ�����Ʈ�� Component / Script �߰�
    void AddComponent(COMPONENT_TYPE _type);        
    void AddScript(const wstring& _strScriptName);  
    void AddPrefab();
    
    void DeleteSelectedObj();

    // CreateMtrl ���� �Լ�
    void SelectShader(DWORD_PTR _key);

    void CollisionSetting();

    void LayerSetting();
};

