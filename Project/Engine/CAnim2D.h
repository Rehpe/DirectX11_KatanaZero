#pragma once
#include "CEntity.h"

#include "ptr.h"
#include "CTexture.h"

// CAnimator2D가 관리하는 애니메이션 클래스, 애니메이션의 프레임을 관리한다.

class CAnimator2D;

class CAnim2D :
    public CEntity
{
public:
    CAnim2D();
    ~CAnim2D();

public:
    CLONE(CAnim2D);

    friend class CAnimator2D;

private:
    CAnimator2D*        m_pOwner;       // 해당 애니메이션을 관리하는 애니메이터
    wstring             m_RelativePath; // 해당 애니메이션 저장 경로
    vector<tAnim2DFrm>  m_vecFrm;       // 프레임 정보 벡터
    Vec2                m_vBackSizeUV;  // 실제 Slice보다 넉넉하게 잡을 뒷배경 크기
    Ptr<CTexture>       m_AtlasTex;     // 애니메이션의 아틀라스 이미지

    Vec2                m_vAtlasResolution;   // 아틀라스 이미지 해상도

    int                 m_iCurFrm;      // 현재 프레임
    
    int                 m_iFPS;         // FPS
    Vec2                m_vBackSize;    // BackSize(픽셀)

    float               m_fTime;        // 누적 시간

    bool                m_bFinish;      // 애니메이션 종료 여부

public:
    void finaltick();

public:
    void Create(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _BackSize, int _FrameCount, int _FPS);

public:
    const tAnim2DFrm& GetCurFrame() { if (m_iCurFrm >= m_vecFrm.size()) m_iCurFrm--; return m_vecFrm[m_iCurFrm]; }
    const tAnim2DFrm& GetFrame(int _i) { return m_vecFrm[_i]; }

    void AddFrame(tAnim2DFrm _frm);

    int GetCurFrameint()            { return m_iCurFrm; }
    void SetCurFrame(int _i)        { m_iCurFrm = _i; }
    void SetBackSizeUV(Vec2 _vUV)   { m_vBackSizeUV = _vUV; }
    Vec2 GetBackSizeUV()            { return m_vBackSizeUV; }
    
    void SetBackSize(Vec2 _vUV)   { m_vBackSize = _vUV; }
    Vec2 GetBackSize()              { return m_vBackSize; }
    Ptr<CTexture> GetAtlasTex()     { return m_AtlasTex; }

    int GetTotalFrm() { return (int)m_vecFrm.size(); }

    int GetFPS() { return m_iFPS; }
    void SetFPS(int _i) { if (_i < 0) _i = 1; m_iFPS = _i; }

    Vec2 GetAtlasResolution() { return m_vAtlasResolution; }

    void SetCurFrmLeftTopUV(Vec2 _vUV) { if (m_iCurFrm >= m_vecFrm.size()) m_iCurFrm--; m_vecFrm[m_iCurFrm].LeftTopUV = _vUV; }
    void SetCurFrmSliceUV(Vec2 _vUV) { if (m_iCurFrm >= m_vecFrm.size()) m_iCurFrm--; m_vecFrm[m_iCurFrm].SliceUV = _vUV; }
    void SetCurFrmOffset(Vec2 _vOffset) { if (m_iCurFrm >= m_vecFrm.size()) m_iCurFrm--; m_vecFrm[m_iCurFrm].OffsetUV = _vOffset; }
    void SetCurFrmDuration(float _fDur) { if (m_iCurFrm >= m_vecFrm.size()) m_iCurFrm--; m_vecFrm[m_iCurFrm].fDuration = _fDur; }

    void SetTotalDuration(float _fDur) {for (size_t i = 0; i < m_vecFrm.size(); i++) { m_vecFrm[i].fDuration = _fDur; }}

    Vec2 GetCurFrmSliceUV() { return m_vecFrm[m_iCurFrm].SliceUV; }

    bool IsFinish() { return m_bFinish; }
    void Reset()
    {
        m_iCurFrm = 0;
        m_fTime = 0.f;
        m_bFinish = false;
    }

    void Clear() { m_vecFrm.clear(); }
    bool IsClear() { return m_vecFrm.empty(); }

    void FrmPopBack() { m_vecFrm.pop_back(); }
    void DeleteFrm(int _i) { vector<tAnim2DFrm>::iterator iter = m_vecFrm.begin() + _i; m_vecFrm.erase(iter); }

    void Save();
    void Load();
    void Load(const wstring& _strFilePath);

    wstring GetRelativePath() { return m_RelativePath; }
    void SetRelativePath(wstring _path) { m_RelativePath = _path;}
};

