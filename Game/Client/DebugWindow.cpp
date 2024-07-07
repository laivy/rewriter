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
		okButton->OnButtonClick->Register(this, std::bind(&RegisterAccountModal::OnButtonClicked, this, ButtonID::OK));
		okButton->SetSize({ 80, 20 });
		okButton->SetPosition({ m_size.x / 2 - 50, m_size.y / 2 + 120 }, Pivot::CENTER);
		okButton->SetText(L"OK");
		m_controls.push_back(okButton);

		auto cancleButton{ std::make_shared<Button>(this) };
		cancleButton->OnButtonClick->Register(this, std::bind(&RegisterAccountModal::OnButtonClicked, this, ButtonID::CANCLE));
		cancleButton->SetSize({ 80, 20 });
		cancleButton->SetPosition({ m_size.x / 2 + 50, m_size.y / 2 + 120 }, Pivot::CENTER);
		cancleButton->SetText(L"CANCLE");
		m_controls.push_back(cancleButton);
	}

	virtual void Render() const override final
	{
		RECTI rect{ 0, 0, m_size.x, m_size.y };
		Renderer2D::DrawRect(rect);
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
			auto callback = [](IModal::Return retval) { };
			std::shared_ptr<IModal> modal{ std::make_shared<RegisterAccountModal>(callback) };
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

	IWindow::Render();
}

void DebugWindow::OnPacket(Packet& packet)
{
}
