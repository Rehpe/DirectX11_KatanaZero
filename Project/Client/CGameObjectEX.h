#pragma once

// 레이어 지정을 하지 않으면서 렌더링을 하기 위한 디버그 전용 게임 오브젝트. CGameObject를 상속받았다

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

