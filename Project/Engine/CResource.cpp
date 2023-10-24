#include "pch.h"
#include "CResource.h"

CResource::CResource(RES_TYPE _type, bool _bEngine)
	: m_Type(_type)
	, m_iRefCount(0)
	, m_bEngine(_bEngine)
{
}

CResource::CResource(const CResource& _Other)
	: CEntity(_Other)
	, m_Type(_Other.m_Type)
	, m_iRefCount(0)					// ���� ����� ���ҽ��� ���� ī��Ʈ�� ������ 0�̾���Ѵ�. 
	, m_strKey(_Other.m_strKey)
	, m_strRelativePath(_Other.m_strRelativePath)
	, m_bEngine(_Other.m_bEngine)
{
}

CResource::~CResource()
{
}

void CResource::Release()
{
	--m_iRefCount;

	if (0 == m_iRefCount)
	{
		delete this;
	}
}
