#include "Stdafx.h"
#include "EditCtrl.h"
#include "NytApp.h"
#include "Wnd.h"
#include "FontPool.h"

EditCtrl::EditCtrl(FLOAT width, FLOAT height, FLOAT x, FLOAT y)
{
	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ x, y });

	if (FontPool::IsInstanced())
		m_textFormat = FontPool::GetInstance()->GetFont(FontType::MORRIS12);
	SetText(TEXT("안녕하세요"));
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

	if (m_textLayout)
	{
		ComPtr<ID2D1SolidColorBrush> textBrush{};
		renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::White }, &textBrush);
		renderTarget->DrawTextLayout(FLOAT2{ 0.0f, 0.0f }, m_textLayout.Get(), textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}
}

RECTF EditCtrl::GetRect() const
{
	return RECTF{};
}

void EditCtrl::SetText(const std::wstring& text)
{
	m_text = text;

	auto dwriteFactory{ NytApp::GetInstance()->GetDwriteFactory() };
	dwriteFactory->CreateTextLayout(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_textFormat.Get(), m_size.x, m_size.y, &m_textLayout);
}

void EditCtrl::SetFont(FontType fontType)
{
	if (FontPool::IsInstanced())
		m_textFormat = FontPool::GetInstance()->GetFont(fontType);
}
