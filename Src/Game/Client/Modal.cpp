#include "Stdafx.h"
#include "Control.h"
#include "Modal.h"

IModal::IModal(std::wstring_view path) :
	IWindow{ path }
{
}

void IModal::Return(IModal::Result ret)
{
	if (OnReturn)
		OnReturn(ret);
	Destroy();
}
