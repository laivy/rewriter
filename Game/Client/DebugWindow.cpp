#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Control.h"
#include "DebugWindow.h"
#include "Modal.h"
#include "Renderer2D.h"
#include "ServerManager.h"
#include "TextBlock.h"
#include "TextBox.h"
#include "WindowManager.h"

DebugWindow::DebugWindow()
{
	m_size = { 600, 400 };
	
	auto idTextBlock{ std::make_shared<TextBlock>(this) };
	idTextBlock->SetSize({ 20, 30 });
	idTextBlock->SetPosition({ m_size.x / 2 - 90, m_size.y / 2 - 22 }, Pivot::CENTER);
	idTextBlock->SetText(L"ID");
	m_controls.push_back(idTextBlock);

	auto idTextBox{ std::make_shared<TextBox>(this) };
	idTextBox->SetSize({ 180, 30 });
	idTextBox->SetPosition({ m_size.x / 2 - 80, m_size.y / 2 - 25 }, Pivot::LEFTCENTER);
	m_controls.push_back(idTextBox);

	auto pwTextBlock{ std::make_shared<TextBlock>(this) };
	pwTextBlock->SetSize({ 25, 30 });
	pwTextBlock->SetPosition({ m_size.x / 2 - 96, m_size.y / 2 + 17 }, Pivot::CENTER);
	pwTextBlock->SetText(L"PW");
	m_controls.push_back(pwTextBlock);

	auto pwTextBox{ std::make_shared<TextBox>(this) };
	pwTextBox->SetSize({ 180, 30 });
	pwTextBox->SetPosition({ m_size.x / 2 - 80, m_size.y / 2 + 15 }, Pivot::LEFTCENTER);
	m_controls.push_back(pwTextBox);

	auto registerButton{ std::make_shared<Button>(this) };
	registerButton->SetSize({ 97, 30 });
	registerButton->SetPosition({ m_size.x / 2 - 100, m_size.y / 2 + 55 }, Pivot::LEFTCENTER);
	registerButton->SetText(L"Register");
	registerButton->OnButtonClick->Register(this,
		[button = registerButton]()
		{
			button->m_state = Button::State::DEFAULT;
			auto callback = [](IModal::Return retval)
				{

				};
			auto modal{ std::make_shared<IModal>(callback) };
			if (auto wm{ WindowManager::GetInstance() })
				wm->Register(modal);
		});
	m_controls.push_back(registerButton);

	auto loginButton{ std::make_shared<Button>(this) };
	loginButton->SetSize({ 97, 30 });
	loginButton->SetPosition({ m_size.x / 2 + 100, m_size.y / 2 + 55 }, Pivot::RIGHTCENTER);
	loginButton->SetText(L"Login");
	loginButton->OnButtonClick->Register(this,
		[]()
		{
			::OutputDebugString(L"OnButtonClick!\n");

			Packet packet{ Packet::Type::CLIENT_TryLogin };
			for (int i = 0; i < 1000; ++i)
				packet.Encode(i);
			packet.End();
			ServerManager::GetInstance()->SendPacket(Server::Type::LOGIN, packet);
		});
	m_controls.push_back(loginButton);

	App::OnPacket->Register(this, std::bind_front(&DebugWindow::OnPacket, this));
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

void DebugWindow::OnPacket(Packet& packet)
{
	switch (packet.GetType())
	{
	case Packet::Type::LOGIN_TryLogin:
	{
		auto str{ packet.Decode<std::string>() };
		break;
	}
	}
}
