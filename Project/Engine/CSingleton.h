#pragma once

typedef void (*EXIT)(void);

template<typename T>
class CSingleton
{
public:
	CSingleton() {};
	virtual ~CSingleton() {};

private:
	static T* m_Inst;

public:
	static	T*	GetInst();
	static void Destroy();

};

template<typename T>
T* CSingleton<T>::m_Inst = nullptr;

template<typename T>
inline T* CSingleton<T>::GetInst()
{
	if (m_Inst == nullptr)
	{
		m_Inst = new T;
		atexit((EXIT) & CSingleton<T>::Destroy);
	}

	return m_Inst;
}

template<typename T>
inline void CSingleton<T>::Destroy()
{
	if (m_Inst != nullptr)
	{
		delete m_Inst;
		m_Inst = nullptr;
	}
}
