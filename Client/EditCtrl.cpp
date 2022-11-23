#include "Stdafx.h"
#include "EditCtrl.h"
#include "Wnd.h"

EditCtrl::EditCtrl(FLOAT width, FLOAT height, FLOAT x, FLOAT y)
{
	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ x, y });
}

void EditCtrl::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	if (!m_parent) return;

	FLOAT2 position{ m_position };
	position += m_parent->GetPosition();

	ComPtr<ID2D1SolidColorBrush> brush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Purple }, &brush);
	renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(position.x, position.y));
	renderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, m_size.x, m_size.y), brush.Get());
}

RECTF EditCtrl::GetRect() const
{
	return RECTF{};
}
