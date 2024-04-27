#pragma once
#include "Window.h"

class IModal : public IWindow
{
public:
	enum class Return
	{
		NONE, OK, CANCLE, YES, NO
	};

	using Callback = std::function<void(Return)>;

public:
	IModal(const Callback& callback);
	~IModal();

protected:
	Return m_return;

private:
	Callback m_callback;
};