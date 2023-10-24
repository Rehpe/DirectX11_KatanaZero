#pragma once
#include <Engine\CScript.h>

class CGameManagerScript :
    public CScript
{
public:
    CGameManagerScript();
    ~CGameManagerScript();

public:
    CLONE(CGameManagerScript);

private:
    bool            m_bInit;
  
    float           m_fTimer;
    float           m_fTimeLimit;
    float           m_fSlowModeCoolDown;
    float           m_fSlowModeTimer;
    bool            m_bIsAllEnemyDead;
    bool            m_bLevelClear;

    bool            m_bCheatMode;

    float           m_fMapOpacity;  
    float           m_fFadeOpacity;

    CGameObject*     m_StageNoti;
    CGameObject*     m_PostProcess;
    CGameObject*     m_FadeInOut;
    CGameObject*     m_EndWall;

    vector<wstring>  m_LevelPath;
    int              m_iLevelIdx;

    bool            m_bSlowActivated;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void    init();
    bool    IsCheatMode() { return m_bCheatMode; }
    void    SetLevelClear(bool _b) { m_bLevelClear = _b; }

    void    FadeIn(CGameObject* _Obj, float& _Opacity, float _MaxBright, float _Speed);  // ¾îµÎ¿ò->¹àÀ½
    void    FadeOut(CGameObject* _Obj, float& _Opacity, float _MaxDark, float _Speed); // ¹àÀ½->¾îµÎ¿ò

    void    UIFadeIn();
    void    UIFadeOut();

    void    GetCurLevelIdx();

    bool    IsFadeInEnd();
    bool    IsFadeOutEnd();

public:
    float   GetMaxTimeLimit() { return m_fTimeLimit; }
    float   GetTimer() { return m_fTimer; }

    float   GetSlowModeCoolDown() { return m_fSlowModeCoolDown; }
    float   GetSlowModeTimer() { return m_fSlowModeTimer; }

public:
    void    TurnBGM();
    void    InitPlayerPos();

};

