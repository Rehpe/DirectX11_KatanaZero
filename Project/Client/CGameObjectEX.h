#pragma once

// ���̾� ������ ���� �����鼭 �������� �ϱ� ���� ����� ���� ���� ������Ʈ. CGameObject�� ��ӹ޾Ҵ�

#include <Engine\CGameObject.h>

class CGameObjectEX :
    public CGameObject
{
public:
    CGameObjectEX();
    ~CGameObjectEX();

private:

public:
    virtual void finaltick() override;

};

