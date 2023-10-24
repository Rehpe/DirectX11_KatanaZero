#pragma once
#include "CSingleton.h"

#include "ptr.h"
#include "CTexture.h"

// ī�޶� ���� �� ������Ʈ�� ������ ���

class CCamera;
class CLight2D;
class CStructuredBuffer;

class CRenderManager :
    public CSingleton<CRenderManager>
{
    SINGLETON(CRenderManager);

private:
    vector<CCamera*>            m_vecCam;       // ī�޶� ������Ʈ�� ���� ���� �� ������ �ε����� �ٿ� ���ͷ� �����Ѵ�
    CCamera*                    m_pEditorCam;   // ������ ī�޶�(������ Stop/Pause ������ �� �ܺθ� ���� ���� ī�޶�)
    
    vector<tDebugShapeInfo>     m_vecShapeInfo;

    // �����Ŵ����� LightInfo�� ������ ���̴�.
    vector<tLightInfo>          m_vecLight2D;
    CStructuredBuffer*          m_Light2DBuffer;

    void (CRenderManager::*RENDER_FUNC)(void);     // ���� ����(Stop/Play)�� ���� � ī�޶� �������� ������� �б�ó��

    Ptr<CTexture>               m_RTCopyTex;    // PostProcess��, ����Ÿ�� ������� �ؽ�ó

public:
    void init();
    void render();

public:
    int  RegisterCamera(CCamera* _Cam, int _idx);
    void RegisterEditorCamera(CCamera* _Cam) { m_pEditorCam = _Cam; }

    void SetRenderFunc(bool _IsPlay);

    void RegisterLight2D(const tLightInfo& _Light2D) { m_vecLight2D.push_back(_Light2D); }
    void ClearCamera();

    void AddDebugShapeInfo(const tDebugShapeInfo& _info) { m_vecShapeInfo.push_back(_info); }
    vector<tDebugShapeInfo>& GetDebugShapeInfo() { return m_vecShapeInfo; }
     
    CCamera* GetMainCam()           // ������ 0�� ī�޶� ���� ī�޶� �ȴ�
    {
        if (m_vecCam.empty())
            return nullptr;

        return m_vecCam[0];
    }

    void CopyRenderTarget();

private:
    void UpdateData();
    void render_play();
    void render_editor();
    void Clear();
};

