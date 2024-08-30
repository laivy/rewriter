#include "Stdafx.h"
#include "Object.h"

IObject::IObject() :
	m_depth{ 0 }
{
}

void IObject::SetDepth(unsigned int depth)
{
	m_depth = depth;
}

unsigned int IObject::GetDepth() const
{
	return m_depth;
}
