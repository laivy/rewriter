#pragma once
#include "UI.h"

class IWindow;

class IControl : public IUserInterface
{
public:
	IControl(IWindow* owner);
	~IControl() = default;

	void SetName(std::wstring_view name);

	bool IsEnable() const;
	std::wstring_view GetName() const;

protected:
	IWindow* m_owner; // 이 컨트롤을 보유하고 있는 윈도우
	bool m_isEnable;
	std::wstring m_name;
};
