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

DebugWindow::DebugWindow()
{
	SetSize(INT2{ 900, 300 });
	if (auto layer{ GetLayer(0) })
	{
		layer->Begin();

		auto root{ Resource::Get(L"UI.dat") };
		auto ninePatch{ root->Get(L"Basic/NinePatch") };

		auto lt{ ninePatch->GetSprite(L"lt") };
		Graphics::D2D::DrawSprite(lt, FLOAT2{});

		auto rt{ ninePatch->GetSprite(L"rt") };
		Graphics::D2D::DrawSprite(rt, FLOAT2{ m_size.x - rt->GetSize().x, 0.0f });

		auto t{ ninePatch->GetSprite(L"t") };
		Graphics::D2D::DrawSprite(t, RECTF{ lt->GetSize().x, 0.0f, m_size.x - rt->GetSize().x, t->GetSize().y });

		auto lb{ ninePatch->GetSprite(L"lb") };
		Graphics::D2D::DrawSprite(lb, FLOAT2{ 0.0f, m_size.y - lb->GetSize().y });

		auto rb{ ninePatch->GetSprite(L"rb") };
		Graphics::D2D::DrawSprite(rb, FLOAT2{ m_size.x - rb->GetSize().x, m_size.y - rb->GetSize().y });

		auto b{ ninePatch->GetSprite(L"b") };
		Graphics::D2D::DrawSprite(b, RECTF{ lb->GetSize().x, m_size.y - b->GetSize().y, m_size.x - b->GetSize().x, static_cast<float>(m_size.y) });

		auto l{ ninePatch->GetSprite(L"l") };
		Graphics::D2D::DrawSprite(l, RECTF{ 0.0f, lt->GetSize().y, l->GetSize().x, m_size.y - lb->GetSize().y });

		auto r{ ninePatch->GetSprite(L"r") };
		Graphics::D2D::DrawSprite(l, RECTF{ m_size.x - r->GetSize().x, rt->GetSize().y, static_cast<float>(m_size.x), m_size.y - rb->GetSize().y });

		auto c{ ninePatch->GetSprite(L"c") };
		Graphics::D2D::DrawSprite(c, RECTF{ lt->GetSize().x, lt->GetSize().y, m_size.x - rb->GetSize().x, m_size.y - rb->GetSize().y });

		layer->End();
	}
	if (auto layer{ GetLayer(1) })
	{
		auto button{ std::make_shared<Button>(this) };
		button->SetZ(1);
		button->SetSize(INT2{ 200, 50 });
		button->SetPosition(INT2{ 450, 150 }, Pivot::Center);
		Register(button);
	}
	if (auto layer{ GetLayer(2) })
	{
	}
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

static float sum{ 0.0f };
void DebugWindow::Update(float deltaTime)
{
	sum = deltaTime;
	IWindow::Update(deltaTime);
}

void DebugWindow::Render() const
{
	if (auto layer{ GetLayer(2) })
	{
		layer->Begin();
		layer->Clear();
		constexpr Graphics::D2D::Font font{ L"", 64.0f };
		Graphics::D2D::DrawText(std::format(L"{}FPS", static_cast<int>(1.0f / sum) ), font, Graphics::D2D::Color::Black, FLOAT2{ 450.0f, 150.0f }, Pivot::Center);
		layer->End();
	}
	IWindow::Render();
}

void DebugWindow::OnPacket(Packet& packet)
{
}
