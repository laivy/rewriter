#include "Stdafx.h"
#include "Object.h"

void IObject2D::SetZ(int z)
{
	m_z = z;
}

int IObject2D::GetZ() const
{
	return m_z;
}
