#pragma once
#include "CComponent.h"

#include "ptr.h"
#include "CTexture.h"

class CAnim2D;

class CAnimator2D :
    public CComponent
{
public:
    CAnimator2D();
    CAnimator2D(const CAnimator2D& _Other);
    ~CAnimator2D();

public:
    CLONE(CAnimator2D);

private:
    map<wstring, CAnim2D*>  m_mapAnim;  // Animation 목록
    CAnim2D*                m_pCurAnim; // 현재 재생중인 Animation
    bool                    m_bRepeat;  // 반복

    vector<Ptr<CTexture>>   m_vecCurAnimFrameImg;
    bool                    m_bPause;

public:
    virtual void finaltick() override;
    void UpdateData();
    void Clear();

public:
    void CreateAnimation(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _vBackSize, int _FrameCount, int _FPS);
    void Play(const wstring& _strName, bool _Repeat);
    void Play(CAnim2D* _anim, bool _Repeat);
    void Pause();

    bool IsPause() { return m_bPause; }

    bool IsRepeat() { return m_bRepeat; }
    void SetRepeat(bool _b) { m_bRepeat = _b; }

    const map<wstring, CAnim2D*>& GetAnimMap() { return m_mapAnim; }
    CAnim2D* FindAnim(const wstring& _strName);
    CAnim2D* GetCurAnim() { return m_pCurAnim;  }
    const vector<Ptr<CTexture>>& GetFrameImg() { return m_vecCurAnimFrameImg; }


    void InsertAnim(CAnim2D* _CurAnim);
    void SetCurAnim(CAnim2D* _CurAnim);
    void RemoveAnim(const wstring _AnimName);

    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

};

