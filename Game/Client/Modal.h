#pragma once
#include "Window.h"

class IModal : public IWindow
{
public:
	enum class Result
	{
		None,
		Ok,
		Cancle,
		Yes,
		No
	};

	using Callback = std::function<void(Result)>;

public:
	IModal(std::wstring_view path);
	~IModal() = default;

	void Return(Result ret);

public:
	Callback OnReturn;
};
