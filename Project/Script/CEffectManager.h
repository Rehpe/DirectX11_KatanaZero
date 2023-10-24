#pragma once

// 생성/소멸자, 멤버변수 없이 오로지 정적멤버함수만 가지는 클래스
class CPrefab;


class CEffectManager
{
public:
	static void CreateEffect(EffectType type, Vec3 pos, bool Destroy);
};

