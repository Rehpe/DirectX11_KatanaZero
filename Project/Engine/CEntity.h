#pragma once

// 매니저(싱글톤) 클래스를 제외한 모든 클래스들은 이 Entity 클래스를 상속받는다.

class CEntity
{
public:
	CEntity();
	CEntity(const CEntity& _other);
	virtual ~CEntity();

private:
	static UINT g_iNextID;

private:
	wstring m_strName;
	const UINT m_iID;

public:
	virtual CEntity* Clone() = 0;

public:
	void SetName(const wstring& _strName) { m_strName = _strName; }
	const wstring& GetName() { return m_strName; }
	UINT GetID() { return m_iID; }

};

