#include "Stdafx.h"
#include "Object.h"

IObject::IObject() :
	m_isValid{ true }
{
}

void IObject::Destroy()
{
	m_isValid = false;
}

bool IObject::IsValid() const
{
	return m_isValid;
}

IObject2D::IObject2D() :
	m_z{ 0 }
{
}

void IObject2D::SetZ(int z)
{
	m_z = z;
}

int IObject2D::GetZ() const
{
	return m_z;
}
