#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Control.h"
#include "DebugWindow.h"
#include "Modal.h"
#include "SocketManager.h"
#include "TextBlock.h"
#include "TextBox.h"
#include "WindowManager.h"

//auto root{ Resource::Get(L"test.dat") };

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
		SetPosition({ App::size.x / 2, App::size.y / 2 }, Pivot::CENTER);

		auto okButton{ std::make_shared<Button>(this) };
		okButton->OnButtonClick.Register(this, std::bind(&RegisterAccountModal::OnButtonClicked, this, ButtonID::OK));
		okButton->SetSize({ 80, 20 });
		okButton->SetPosition({ m_size.x / 2 - 50, m_size.y / 2 + 120 }, Pivot::CENTER);
		okButton->SetText(L"OK");
		Register(okButton);

		auto cancleButton{ std::make_shared<Button>(this) };
		cancleButton->OnButtonClick.Register(this, std::bind(&RegisterAccountModal::OnButtonClicked, this, ButtonID::CANCLE));
		cancleButton->SetSize({ 80, 20 });
		cancleButton->SetPosition({ m_size.x / 2 + 50, m_size.y / 2 + 120 }, Pivot::CENTER);
		cancleButton->SetText(L"CANCLE");
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

DebugWindow::DebugWindow()
{
	SetSize(INT2{ 800, 600 });
	GetLayer(0);
	App::OnPacket.Register(this, std::bind_front(&DebugWindow::OnPacket, this));
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
	auto root{ Resource::Get(L"test.dat") };

	Graphics::D2D::Font font{ L"", 128.0f };
	DrawText(L"안녕하세요", { 50.0f, 50.0f }, font, Graphics::D2D::Color::Black);

	IWindow::Render();
}

void DebugWindow::OnPacket(Packet& packet)
{
}
