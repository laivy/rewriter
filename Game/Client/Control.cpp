#include "stdafx.h"
#include "Control.h"

IControl::IControl(IWindow* owner) :
	m_owner{ owner },
	m_isEnable{ true }
{
}

bool IControl::IsEnable() const
{
	return m_isEnable;
}
