#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Control.h"
#include "LoginServer.h"
#include "LoginWindow.h"
#include "Modal.h"
#include "SocketManager.h"
#include "TextBlock.h"
#include "TextBox.h"
#include "WindowManager.h"

class RegisterAccountModal :
	public IModal,
	public IObserver
{
private:
	enum class ButtonID
	{
		OK, CANCLE
	};

public:
	RegisterAccountModal(const Callback& callback) :
		IModal{ callback }
	{
		m_size = { 500, 300 };
		SetPosition({ App::size.x / 2, App::size.y / 2 }, Pivot::Center);

		auto okButton{ std::make_shared<Button>(this) };
		okButton->OnButtonClick.Register(this, std::bind(&RegisterAccountModal::OnButtonClicked, this, ButtonID::OK));
		okButton->SetSize({ 80, 20 });
		okButton->SetPosition({ m_size.x / 2 - 50, m_size.y / 2 + 120 }, Pivot::Center);
		Register(okButton);

		auto cancleButton{ std::make_shared<Button>(this) };
		cancleButton->OnButtonClick.Register(this, std::bind(&RegisterAccountModal::OnButtonClicked, this, ButtonID::CANCLE));
		cancleButton->SetSize({ 80, 20 });
		cancleButton->SetPosition({ m_size.x / 2 + 50, m_size.y / 2 + 120 }, Pivot::Center);
		Register(cancleButton);
	}

	virtual void Render() const override final
	{
		RECTI rect{ 0, 0, m_size.x, m_size.y };
		Graphics::D2D::DrawRect(rect, Graphics::D2D::Color::White);
		IWindow::Render();
	}

private:
	void OnButtonClicked(ButtonID id)
	{
		if (id == ButtonID::CANCLE)
			Destroy();
	}
};

LoginWindow::LoginWindow() :
	IWindow{ L"UI.dat/Login" }
{
	SetPosition(App::size / 2, Pivot::Center);
	if (auto button{ GetControl<Button>(L"Login") })
		button->OnButtonClick.Register([this]() { OnLoginButtonClicked(); });
	if (auto button{ GetControl<Button>(L"Register") })
		button->OnButtonClick.Register([this]() { OnRegisterButtonClicked(); });
	LoginServer::GetInstance()->OnPacket.Register(this, std::bind_front(&LoginWindow::OnPacket, this));
}

void LoginWindow::OnMouseEvent(UINT message, int x, int y)
{
	IWindow::OnMouseEvent(message, x, y);
}

void LoginWindow::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	IWindow::OnKeyboardEvent(message, wParam, lParam);
}

void LoginWindow::Update(float deltaTime)
{
	IWindow::Update(deltaTime);
}

void LoginWindow::Render() const
{
	IWindow::Render();
}

void LoginWindow::OnPacket(Packet& packet)
{
	switch (packet.GetType())
	{
	case Packet::Type::LoginResult:
	{
		bool isSuccess{ packet.Decode<bool>() };
		break;
	}
	default:
		break;
	}
}

void LoginWindow::OnLoginButtonClicked()
{
	Packet packet{ Packet::Type::RequestLogin };
	if (auto textBox{ GetControl<TextBox>(L"ID") })
		packet.Encode(textBox->GetText());
	else
		packet.Encode(L"");
	if (auto textBox{ GetControl<TextBox>(L"Password") })
		packet.Encode(textBox->GetText());
	else
		packet.Encode(L"");
	LoginServer::GetInstance()->Send(packet);
}

void LoginWindow::OnRegisterButtonClicked()
{
}
