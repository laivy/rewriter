#include "Stdafx.h"
#include "Modal.h"

IModal::IModal(const Callback& callback) : 
	m_return{ Return::NONE },
	m_callback{ callback }
{
}

IModal::~IModal()
{
	if (m_callback)
		m_callback(m_return);
}