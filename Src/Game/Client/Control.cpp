#include "stdafx.h"
#include "Control.h"

IControl::IControl(IWindow* owner) :
	m_owner{ owner },
	m_isEnable{ true }
{
}

void IControl::SetEnable(bool enable)
{
	m_isEnable = enable;
}

void IControl::SetName(std::wstring_view name)
{
	m_name = name;
}

bool IControl::IsEnable() const
{
	return m_isEnable;
}

std::wstring_view IControl::GetName() const
{
	return m_name;
}
