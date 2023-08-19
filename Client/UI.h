#pragma once

class Wnd;

class IUserInterface abstract
{
public:
	IUserInterface();
	virtual ~IUserInterface() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;
	virtual void Destroy();

	void SetParent(Wnd* const wnd);
	void SetFocus(bool focus);
	void SetPivot(Pivot pivot);
	void SetSize(const INT2& size);
	void SetPosition(const FLOAT2& position);

	Wnd* const GetParent() const;
	bool IsValid() const;
	bool IsFocus() const;
	RECTF GetRect() const;
	INT2 GetSize() const;
	FLOAT2 GetPosition() const;

protected:
	Wnd* m_parent;
	bool m_isValid;
	bool m_isFocus;
	Pivot m_pivot;
	INT2 m_size;
	FLOAT2 m_position;
};