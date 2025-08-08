#pragma once
#include "Object.h"

class IUserInterface : public IObject2D
{
public:
	IUserInterface();
	virtual ~IUserInterface() = default;
	
	virtual void Update(float deltaTime) override;
	virtual void Render() const override;

	virtual void OnMouseEnter(int x, int y);
	virtual void OnMouseLeave(int x, int y);
	virtual void OnMouseEvent(UINT message, int x, int y);
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void SetFocus(bool focus);
	virtual void SetPosition(const Int2& position, Pivot pivot = Pivot::LeftTop);
	virtual void SetSize(const Int2& size);

	virtual bool IsFocus() const;

	Int2 GetPosition(Pivot pivot = Pivot::LeftTop) const;
	Int2 GetSize() const;
	bool Contains(const Int2& point) const;

protected:
	bool m_isFocus;
	Int2 m_position; // 좌측 상단 좌표
	Int2 m_size;
};
