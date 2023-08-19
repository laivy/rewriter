#pragma once
#include "UI.h"

class Wnd : public IUserInterface
{
public:
	Wnd(const INT2& size);
	virtual ~Wnd() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnButtonClicked(INT id);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext);

	template <typename T>
	requires std::is_base_of_v<IUserInterface, T>
	void AddUI(T* ui)
	{
		ui->SetParent(this);
		m_ui.emplace_back(ui);
	}

	void SetUIFocus(IUserInterface* focusUI);
	void SetFocus(BOOL isFocus);
	void SetPick(BOOL isPick);

	BOOL IsFocus() const;
	BOOL IsPick() const;
	bool IsInWnd(const INT2& point);

private:
	FLOAT2 GetPickedDelta() const;

protected:
	static constexpr float WND_TITLE_HEIGHT = 15.0f;

private:
	std::vector<std::unique_ptr<IUserInterface>> m_ui;
	BOOL m_isFocus;
	BOOL m_isPick;
	FLOAT2 m_pickDelta;
};