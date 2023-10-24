#pragma once
#include "CSingleton.h"

#include "ptr.h"
#include "CTexture.h"

// 카메라 관리 및 오브젝트의 렌더링 담당

class CCamera;
class CLight2D;
class CStructuredBuffer;

class CRenderManager :
    public CSingleton<CRenderManager>
{
    SINGLETON(CRenderManager);

private:
    vector<CCamera*>            m_vecCam;       // 카메라 오브젝트는 여러 대일 수 있으며 인덱스를 붙여 벡터로 관리한다
    CCamera*                    m_pEditorCam;   // 에디터 카메라(레벨이 Stop/Pause 상태일 때 외부를 보기 위한 카메라)
    
    vector<tDebugShapeInfo>     m_vecShapeInfo;

    // 렌더매니저는 LightInfo를 취합할 것이다.
    vector<tLightInfo>          m_vecLight2D;
    CStructuredBuffer*          m_Light2DBuffer;

    void (CRenderManager::*RENDER_FUNC)(void);     // 레벨 상태(Stop/Play)에 따라 어떤 카메라 렌더링을 사용할지 분기처리

    Ptr<CTexture>               m_RTCopyTex;    // PostProcess용, 렌더타겟 복사받을 텍스처

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
     
    CCamera* GetMainCam()           // 언제나 0번 카메라가 메인 카메라가 된다
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

