#pragma once

// Resource는 자체 제작한 스마트 포인터 Ptr 템플릿을 사용한다

template<typename T>
class Ptr
{

private:
	T*			m_Res;

public:
	T* Get() const { return m_Res; }

	T* operator -> () const
	{
		return m_Res;
	}

	bool operator == (T * _Other)
	{
		return m_Res == _Other;
	}

	bool operator != (T * _Other)
	{
		return m_Res != _Other;
	}

	bool operator == (const Ptr<T>& _Other)
	{
		return m_Res == _Other.m_Res;
	}

	bool operator != (const Ptr<T>& _Other)
	{
		return m_Res != _Other.m_Res;
	}

	void	operator = (T* _Res)
	{
		// 대입 이전에 가리키고 있던 리소스가 있었다면 Release
		if (nullptr != m_Res)
			m_Res->Release();

		m_Res = _Res;

		if (nullptr != m_Res)
			m_Res->AddRef();
	}

	void	operator = (const Ptr<T>& _Res)
	{
		// 대입 이전에 가리키고 있던 리소스가 있었다면 Release
		if (nullptr != m_Res)
			m_Res->Release();

		m_Res = _Res.m_Res;

		if (nullptr != m_Res)
			m_Res->AddRef();
	}

public:
	Ptr()
		: m_Res(nullptr)
	{
	};

	Ptr(T* _Res)			// 만들어지자마자 특정 타입이 대입되는 경우 (원형 리소스의 포인터)
		: m_Res(_Res)
	{
		if (nullptr != m_Res)
			m_Res->AddRef();
	}

	Ptr(const Ptr<T>& _Res)		// 같은 Ptr이 대입되는 경우
		: m_Res(_Res.m_Res)
	{
		if (nullptr != m_Res)
			m_Res->AddRef();
	}
	
	~Ptr()
	{
		if (nullptr != m_Res)
		{
			m_Res->Release();
		}
	}

public:
};


// 전역함수 operator

template<typename T>
bool operator == (void* _Res, const Ptr<T>& _Ptr)
{
	return _Res == _Ptr.Get();
}

template<typename T>
bool operator != (void* _Res, const Ptr<T>& _Ptr)
{
	return _Res != _Ptr.Get();
}