#pragma once

class Wnd;

class UI abstract
{
public:
	UI();
	virtual ~UI() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;
	virtual void Destroy();

	void SetParent(Wnd* const wnd);
	void SetFocus(BOOL focus);
	void SetSize(const FLOAT2& size);
	void SetPosition(const FLOAT2& position, Pivot pivot = Pivot::LEFTTOP);

	INT GetId() const;
	BOOL IsValid() const;
	BOOL IsFocus() const;
	RECTF GetRect() const;
	FLOAT2 GetSize() const;
	FLOAT2 GetPosition() const;
	Wnd* const GetParent() const;

protected:
	Wnd* m_parent;
	BOOL m_isValid;
	BOOL m_isFocus;
	FLOAT2 m_position;
	FLOAT2 m_size;

private:
	static INT s_id;
	INT m_id;
};