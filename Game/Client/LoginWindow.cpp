#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Control.h"
#include "LoginServer.h"
#include "LoginWindow.h"
#include "Modal.h"
#include "PopupModal.h"
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
	void OnCheckIDButtonClicked()
	{
		std::wstring id;
		if (auto textBox{ GetControl<TextBox>(L"ID") })
			id = textBox->GetText();

		Packet packet{ Protocol::Type::Register };
		packet.Encode(Protocol::Register::CheckID, id);
		LoginServer::GetInstance()->Send(packet);
	}

	void OnOKButtonClicked()
	{
		std::wstring id;
		std::wstring pw;
		std::wstring pw2;
		if (auto textBox{ GetControl<TextBox>(L"ID") })
			id = textBox->GetText();
		if (auto textBox{ GetControl<TextBox>(L"PW") })
			pw = textBox->GetText();
		if (auto textBox{ GetControl<TextBox>(L"PW2") })
			pw2 = textBox->GetText();
		if (id.empty() || pw.empty() || pw2.empty())
			return;
		if (pw != pw2)
			return;

		Packet packet{ Protocol::Type::Register };
		packet.Encode(Protocol::Register::Request, id, pw);
		LoginServer::GetInstance()->Send(packet);
	}

	void OnCancleButtonClicked()
	{
		Return(Result::Cancle);
	}

	void OnPacket(Packet& packet)
	{
		if (packet.GetType() != Protocol::Type::Register)
			return;

		auto subType{ packet.Decode<Protocol::Register>() };
		switch (subType)
		{
		case Protocol::Register::CheckID:
		{
			auto isAvailable{ packet.Decode<bool>() };
			auto popup{ std::make_shared<PopupModal>(isAvailable ? L"사용 가능한 아이디입니다." : L"사용 불가능한 아이디입니다.") };
			WindowManager::GetInstance()->Register(std::static_pointer_cast<IModal>(popup));
			break;
		}
		default:
			break;
		}
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
	std::wstring id;
	std::wstring pw;
	if (auto textBox{ GetControl<TextBox>(L"ID") })
		id = textBox->GetText();
	if (auto textBox{ GetControl<TextBox>(L"PW") })
		pw = textBox->GetText();
	if (id.empty() || pw.empty())
		return;

	Packet packet{ Protocol::Type::Login };
	packet.Encode(Protocol::Login::Login, id, pw);
	LoginServer::GetInstance()->Send(packet);
}

void LoginWindow::OnRegisterButtonClicked()
{
	auto modal{ std::make_shared<RegisterAccountModal>() };
	WindowManager::GetInstance()->Register(std::static_pointer_cast<IModal>(modal));
}
