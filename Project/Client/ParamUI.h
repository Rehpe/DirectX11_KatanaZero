#pragma once

#include <Engine\ptr.h>
#include <Engine\CTexture.h>

// 재질에서 공개한 상수/텍스처 파라미터를 출력하는 기능을 모아둔 함수 집합체
// 생성, 소멸자 없음, 모든 멤버함수/변수 static으로 선언 ( 전역함수처럼 객체 없이 호출 가능 )

class ParamUI
{
	friend class ImGuiManager;

private:
	static UINT		g_NextID;

public:
	static int Param_Int(const string& _strDesc, int* _pData, bool _bDrag = false);
	static int Param_Float(const string& _strDesc, float* _pData, bool _bDrag = false);
	static int Param_Vec2(const string& _strDesc, Vec2* _pData, bool _bDrag = false);
	static int Param_Vec4(const string& _strDesc, Vec4* _pData, bool _bDrag = false);

	static int Param_Tex(const string& _strDesc, Ptr<CTexture>& _CurTex, UI* _UI, UI_DELEGATE_1 _Func);

private:
	static string GetNextName(const string& _ShowName);
};

