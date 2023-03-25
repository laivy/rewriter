#include "Stdafx.h"
#include "BrushPool.h"
#include "FontPool.h"
#include "NytApp.h"
#include "Label.h"
#include "Wnd.h"

Label::Label(FLOAT width, FLOAT height)
{
	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ 0.0f, 0.0f });
	m_textFormat = FontPool::GetInstance()->GetFont(FontPool::MORRIS);
}

void Label::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (!m_parent) return;

	FLOAT2 position{ m_position };
	position += m_parent->GetPosition();
	d2dContext->SetTransform(MATRIX::Translation(position.x, position.y));

	if (m_textLayout)
	{
		d2dContext->DrawTextLayout(FLOAT2{ 0.0f, 0.0f }, m_textLayout.Get(), BrushPool::GetInstance()->GetBrush(BrushPool::WHITE), D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}
}

void Label::SetText(const std::wstring& text)
{
	m_text = text;
	NytApp::GetInstance()->GetDwriteFactory()->CreateTextLayout(
		m_text.c_str(),
		static_cast<UINT32>(m_text.length()),
		m_textFormat.Get(),
		m_size.x,
		m_size.y,
		&m_textLayout
	);
}
