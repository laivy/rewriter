#pragma once
#include "UI.h"

class Wnd : public IUserInterface
{
protected:
	static constexpr int DEFAULT_PICK_AREA_HEIGHT = 15;

public:
	Wnd(const INT2& size);
	virtual ~Wnd() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnButtonClick(ButtonID id);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext);

	virtual void SetFocus(bool isFocus);

	template <typename T>
	requires std::is_base_of_v<IUserInterface, T>
	void AddUI(T* ui)
	{
		ui->SetParent(this);
		m_userInterfaces.emplace_back(ui);
	}

	void SetUIFocus(IUserInterface* focusUI);
	void SetPick(bool isPick);
	
	bool IsPick() const;
	bool IsInPickArea(const INT2& point);

private:
	std::vector<std::unique_ptr<IUserInterface>> m_userInterfaces;
	bool m_isPick;
	RECTI m_pickArea;
	INT2 m_pickDelta;
};