#pragma once
#include "Object.h"

class IUserInterface abstract : public IObject2D
{
public:
	IUserInterface();
	virtual ~IUserInterface() = default;
	
	virtual void OnMouseEnter(int x, int y);
	virtual void OnMouseLeave(int x, int y);
	virtual void OnMouseEvent(UINT message, int x, int y);
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(float deltaTime) override;
	virtual void Render() const override;
	virtual void Destroy();

	virtual void SetFocus(bool focus);
	virtual void SetPosition(const INT2& position, Pivot pivot = Pivot::LEFTTOP);
	virtual void SetSize(const INT2& size);

	virtual bool IsValid() const;
	virtual bool IsFocus() const;

	bool IsContain(const INT2& point) const;
	INT2 GetPosition(Pivot pivot = Pivot::LEFTTOP) const;
	INT2 GetSize() const;

protected:
	bool m_isValid;
	bool m_isFocus;
	INT2 m_position; // 좌측 상단 좌표
	INT2 m_size;
};