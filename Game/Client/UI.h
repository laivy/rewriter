#pragma once

class Wnd;

class IUserInterface abstract
{
public:
	IUserInterface();
	virtual ~IUserInterface() = default;
	
	virtual void OnMouseMove(int x, int y);
	virtual void OnLButtonUp(int x, int y);
	virtual void OnLButtonDown(int x, int y);
	virtual void OnRButtonUp(int x, int y);
	virtual void OnRButtonDown(int x, int y);
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(float deltaTime);
	virtual void Render() const;
	virtual void Destroy();

	virtual void SetParent(Wnd* const wnd);
	virtual void SetFocus(bool focus);
	virtual void SetSize(const INT2& size);
	virtual void SetPosition(const INT2& position, Pivot pivot = Pivot::LEFTTOP);

	Wnd* const GetParent() const;
	bool IsValid() const;
	bool IsFocus() const;
	bool IsContain(const INT2& point) const;
	INT2 GetSize() const;
	INT2 GetPosition(Pivot pivot = Pivot::LEFTTOP) const;

protected:
	Wnd* m_parent;
	bool m_isValid;
	bool m_isFocus;
	INT2 m_size;
	INT2 m_position; // 좌측 상단 좌표
};