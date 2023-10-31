#include "Stdafx.h"
#include "Button.h"
#include "EditCtrl.h"
#include "LoginWnd.h"
#include "LoginScene.h"
#include "LoginServer.h"
#include "GameScene.h"
#include "SceneManager.h"

LoginWnd::LoginWnd(const INT2& size) : Wnd{ size }
{
	auto editCtrl{ std::make_unique<EditCtrl>(INT2{ 200, 20 }) };
	editCtrl->SetPosition({ size.x / 2, size.y / 2 });
	AddUI(editCtrl.release());

	auto button{ std::make_unique<Button>(INT2{ 200, 20 }) };
	button->SetPosition({ size.x / 2, size.y / 2 + 30 });
	button->SetButtonID(Buttons::LOGIN);
	AddUI(button.release());
}

void LoginWnd::OnButtonClick(ButtonID id)
{
	switch (id)
	{
	case Buttons::LOGIN:
	{
		Packet packet{ Packet::Type::CLIENT_TryLogin };
		packet.Encode(
			std::string{ "id" },
			std::string{ "pw" }
		);
		packet.End();
		LoginServer::GetInstance()->Send(packet);
		break;
	}
	default:
		break;
	}
}

#include "Renderer2D.h"
void LoginWnd::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext)
{
	auto dnf{ Resource::Get("test.dat") };
	auto image{ Resource::GetImage(dnf, "dnf") };
	Renderer2D::DrawImage(image, { 100, 100 });
}