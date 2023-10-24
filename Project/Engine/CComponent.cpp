#include "pch.h"
#include "CComponent.h"

CComponent::CComponent(COMPONENT_TYPE _Type)
	: m_Type(_Type)
	, m_pOwner(nullptr)
{
}

CComponent::CComponent(const CComponent& _Other)
	: CEntity(_Other)
	, m_Type(_Other.m_Type)
	, m_pOwner(nullptr)
{
}

CComponent::~CComponent()
{
}
