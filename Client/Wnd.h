#pragma once
#include "UI.h"

class Wnd : public IUserInterface
{
public:
	Wnd(FLOAT width, FLOAT height, FLOAT x = 0.0f, FLOAT y = 0.0f);
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

	std::mutex& GetLock();
	BOOL IsValid() const;
	BOOL IsFocus() const;
	BOOL IsPick() const;

private:
	FLOAT2 GetPickedDelta() const;

protected:
	static constexpr FLOAT WND_TITLE_HEIGHT = 15.0f;

private:
	std::mutex m_mutex;

	BOOL m_isFocus;
	BOOL m_isPick;
	FLOAT2 m_pickDelta;

	std::vector<std::unique_ptr<IUserInterface>> m_ui;
};