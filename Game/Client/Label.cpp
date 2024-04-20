#include "Stdafx.h"
#include "Label.h"
#include "Renderer2D.h"
#include "Wnd.h"

Label::Label(const INT2& size)
{
	SetSize(size);
	//if (auto rm{ ResourceManager::GetInstance() })
	//{
	//	SetFont(rm->GetFont(Font::Type::MORRIS12));
	//	SetText("");
	//}
}

void Label::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (!m_parent) return;

	INT2 position{ m_position };
	position += m_parent->GetPosition();
	d2dContext->SetTransform(MATRIX::Translation(static_cast<float>(position.x), static_cast<float>(position.y)));

	if (m_textLayout)
	{
		//d2dContext->DrawTextLayout(FLOAT2{ 0.0f, 0.0f }, m_textLayout.Get(), BrushPool::GetInstance()->GetBrush(BrushPool::WHITE), D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}
}

void Label::SetFont(const std::shared_ptr<Font>& font)
{
	m_font = font;
}

void Label::SetText(const std::string& text)
{
	m_text = TextUtil::str2wstr(text);
	Renderer2D::dwriteFactory->CreateTextLayout(
		m_text.c_str(),
		static_cast<UINT32>(m_text.length()),
		m_font->GetTextFormat().Get(),
		static_cast<float>(m_size.x),
		static_cast<float>(m_size.y),
		&m_textLayout
	);
}
