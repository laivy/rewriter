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

class RegisterAccountModal final :
	public IModal,
	public IObserver
{
public:
	RegisterAccountModal() :
		IModal{ L"UI.dat/Register" }
	{
		SetPosition(App::size / 2, Pivot::Center);
		if (auto button{ GetControl<Button>(L"CheckID") })
			button->OnButtonClick.Register([this]() { OnCheckIDButtonClicked(); });
		if (auto button{ GetControl<Button>(L"Ok") })
			button->OnButtonClick.Register([this]() { OnOKButtonClicked(); });
		if (auto button{ GetControl<Button>(L"Cancle") })
			button->OnButtonClick.Register([this]() { OnCancleButtonClicked(); });
		LoginServer::GetInstance()->OnPacket.Register(this, std::bind_front(&RegisterAccountModal::OnPacket, this));
	}

private:
	void OnPacket(Packet& packet)
	{

	}

	void OnCheckIDButtonClicked()
	{
		std::wstring id;
		if (auto textBox{ GetControl<TextBox>(L"ID") })
			id = textBox->GetText();

		Packet packet{ Protocol::AccountRegisterRequest };
		packet.Encode(AccountRegisterRequest::CheckID, id);
		LoginServer::GetInstance()->Send(packet);
	}

	void OnOKButtonClicked()
	{
		std::wstring id;
		std::wstring pw;
		if (auto textBox{ GetControl<TextBox>(L"ID") })
			id = textBox->GetText();
		if (auto textBox{ GetControl<TextBox>(L"Password") })
			pw = textBox->GetText();

		Packet packet{ Protocol::AccountRegisterRequest };
		packet.Encode(AccountRegisterRequest::Request, id, pw);
		LoginServer::GetInstance()->Send(packet);
	}

	void OnCancleButtonClicked()
	{
		Return(IModal::Result::Cancle);
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

void LoginWindow::OnPacket(Packet& packet)
{
}

void LoginWindow::OnLoginButtonClicked()
{
}

void LoginWindow::OnRegisterButtonClicked()
{
	auto modal{ std::make_shared<RegisterAccountModal>() };
	WindowManager::GetInstance()->Register(std::static_pointer_cast<IModal>(modal));
}
