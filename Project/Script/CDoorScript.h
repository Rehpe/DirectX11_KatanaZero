#pragma once
#include <Engine\CScript.h>

enum class DOOR_STATE
{
    CLOSE,
    OPEN,
};

class CDoorScript :
    public CScript
{
public:
    CDoorScript();
    ~CDoorScript();

public:
    CLONE(CDoorScript);

private:
    UINT        m_eDoorState;
    int         m_iDoorIdx;  // 본인이 맵의 몇 번째 Door인지

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other);
    virtual void OnOverlap(CCollider2D* _Other);
    virtual void EndOverlap(CCollider2D* _Other);

public:
    UINT    GetDoorState() { return m_eDoorState; };
    void    SetDoorState(UINT _Doorstate) { m_eDoorState = _Doorstate; };

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};

