#include "Stdafx.h"
#include "Button.h"
#include "Control.h"
#include "DebugWindow.h"
#include "Renderer2D.h"
#include "Server.h"
#include "TextBlock.h"
#include "TextBox.h"

DebugWindow::DebugWindow()
{
	m_size = { 600, 400 };
	
	auto textBlock{ std::make_shared<TextBlock>(this) };
	textBlock->SetSize({ 200, 30 });
	textBlock->SetPosition({ m_size.x / 2, m_size.y / 2 - 25 }, Pivot::CENTER);
	textBlock->SetText(L"Hello, TextBlock!");
	m_controls.push_back(textBlock);

	auto textBox{ std::make_shared<TextBox>(this) };
	textBox->SetSize({ 200, 30 });
	textBox->SetPosition({ m_size.x / 2, m_size.y / 2 }, Pivot::CENTER);
	m_controls.push_back(textBox);

	auto button{ std::make_shared<Button>(this) };
	button->SetSize({ 200, 30 });
	button->SetPosition({ m_size.x / 2, m_size.y / 2 + 25 }, Pivot::CENTER);
	button->SetText(L"Button");
	button->OnButtonClick.Register(this,
		[]()
		{
			::OutputDebugString(L"OnButtonClick!\n");

			Packet packet{ Packet::Type::CLIENT_TryLogin };
			for (int i = 0; i < 1000; ++i)
				packet.Encode(i);
			packet.End();
			Send<LoginServer>(packet);
		});
	m_controls.push_back(button);
}

void DebugWindow::OnMouseEvent(UINT message, int x, int y)
{
	IWindow::OnMouseEvent(message, x, y);
}

void DebugWindow::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	IWindow::OnKeyboardEvent(message, wParam, lParam);
}

void DebugWindow::Update(float deltaTime)
{
	IWindow::Update(deltaTime);
}

void DebugWindow::Render() const
{
	RECTI rect{ 0, 0, m_size.x, m_size.y };
	RECTI outline{ rect };
	outline.left -= 5;
	outline.top -= 5;
	outline.right += 5;
	outline.bottom += 5;
	Renderer2D::DrawRect(outline, D2D1::ColorF::Black);
	Renderer2D::DrawRect(rect, D2D1::ColorF::White);

	for (const auto& control : m_controls)
		control->Render();
}
