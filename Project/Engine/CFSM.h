#pragma once
#include "CComponent.h"

class CState;

class CFSM :
    public CComponent
{
public:
    CFSM();
    CFSM(const CFSM& _other);
    ~CFSM();

public:
    CLONE(CFSM);

private:
    map<wstring, CState*>   m_mapState;
    CState*                 m_pCurState;

public:
    virtual void finaltick() override;

public:
    void    AddState(const wstring& _strKey, CState* _pState);
    CState* FindState(const wstring& _strKey);
    void    ChangeState(const wstring& _strStateName);

    CState* GetCurState() { return m_pCurState; }

public:
    virtual void SaveToLevelFile(FILE* _File) override {};
    virtual void LoadFromLevelFile(FILE* _File) override {};

};

