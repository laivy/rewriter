#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Control.h"
#include "TextBox.h"
#include "Window.h"

namespace
{
	constexpr std::wstring_view INFO{ L"Info" };
	constexpr std::wstring_view NINE_PATCH{ L"NinePatch" };
	constexpr std::wstring_view BUTTON_PREFIX{ L"Button:" };
	constexpr std::wstring_view TEXTBOX_PREFIX{ L"TextBox:" };
}

IWindow::IWindow() :
	m_titleBarRect{},
	m_isPicked{},
	m_pickPos{}
{
}

IWindow::IWindow(std::wstring_view path) :
	IWindow{ Resource::Get(path) }
{
}

IWindow::IWindow(const std::shared_ptr<Resource::Property>& prop) :
	IWindow{}
{
	Build(prop);
}

void IWindow::OnMouseLeave(int x, int y)
{
	if (auto control{ m_mouseOverControl.lock() })
		control->OnMouseLeave(x, y);
	m_mouseOverControl.reset();
}

void IWindow::OnMouseEvent(UINT message, int x, int y)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		UpdateMouseOverControl(x, y);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		// 피킹 됐는지 확인
		if (m_titleBarRect.Contains(Int2{ x, y }))
		{
			m_isPicked = true;
			m_pickPos = { x, y };
		}

		UpdateFocusControl(x, y);
		break;
	}
	case WM_LBUTTONUP:
	{
		m_isPicked = false;
		m_pickPos = { 0, 0 };
		break;
	}
	default:
		break;
	}

	if (auto control{ m_focusControl.lock() })
	{
		auto pos{ control->GetPosition() };
		control->OnMouseEvent(message, x - pos.x, y - pos.y);
	}
}

void IWindow::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			Destroy();
		break;
	}
	default:
		break;
	}

	if (auto control{ m_focusControl.lock() })
		control->OnKeyboardEvent(message, wParam, lParam);
}

void IWindow::Update(float deltaTime)
{
	// 선택된 윈도우 마우스로 옮기기
	if (m_isPicked)
	{
		POINT cursor{};
		::GetCursorPos(&cursor);
		::ScreenToClient(App::hWnd, &cursor);
		SetPosition({
			cursor.x - m_pickPos.x,
			cursor.y - m_pickPos.y
		});
	}
	
	for (const auto& control : m_controls)
		control->Update(deltaTime);
}

void IWindow::Render() const
{
	RenderNinePatch();

	for (const auto& control : m_controls)
	{
		const auto& layer{ GetLayer(control->GetZ()) };
		Graphics::D2D::PushLayer(layer);
		control->Render();
		Graphics::D2D::PopLayer();
	}

	for (const auto& layer : m_layers | std::views::values)
		Graphics::D2D::DrawLayer(layer);
}

void IWindow::Register(const std::shared_ptr<IControl>& control)
{
	// 레이어 생성
	int z{ control->GetZ() };
	if (!m_layers.contains(z))
		m_layers.emplace(z, Graphics::D2D::CreateLayer(GetSize()));

	// 삽입 정렬
	m_controls.insert(std::ranges::upper_bound(m_controls, control, [](const auto& lhs, const auto& rhs) { return lhs->GetZ() < rhs->GetZ(); }), control);
}

std::shared_ptr<Graphics::D2D::Layer> IWindow::GetLayer(int z) const
{
	if (m_layers.contains(z))
		return m_layers.at(z);
	assert(false && "LAYER IS NOT EXIST");
	return nullptr;
}

void IWindow::Build(const std::shared_ptr<Resource::Property>& prop, std::wstring_view path)
{
	if (!prop)
		return;

	for (const auto& [name, child] : *prop)
	{
		if (name.starts_with(INFO))
		{
			SetInfo(child);
		}
		else if (name.starts_with(NINE_PATCH))
		{
			SetNinePatch(child);
		}
		else if (name.starts_with(BUTTON_PREFIX))
		{
			auto button{ std::make_shared<Button>(this, child) };
			std::wstring controlName{ name.substr(BUTTON_PREFIX.size()) };
			if (!path.empty())
				controlName = std::format(L"{}/{}", path, controlName);
			button->SetName(controlName);
			Register(button);
		}
		else if (name.starts_with(TEXTBOX_PREFIX))
		{
			auto textBox{ std::make_shared<TextBox>(this, child) };
			std::wstring controlName{ name.substr(TEXTBOX_PREFIX.size()) };
			if (!path.empty())
				controlName = std::format(L"{}/{}", path, controlName);
			textBox->SetName(controlName);
			Register(textBox);
		}

		Build(child, std::format(L"{}/{}", path, name));
	}
}

void IWindow::SetInfo(const std::shared_ptr<Resource::Property>& prop)
{
	/*
	- Size(Int2)
		- 윈도우 크기
	- TitleBarLT(Int2), TitleBarRB(Int2)
		- 타이틀바(드래그 영역) 좌상단, 우하단 좌표
	*/

	SetSize(prop->GetInt2(L"Size"));

	auto titleBarLT{ prop->GetInt2(L"TitleBarLT") };
	auto titleBarRB{ prop->GetInt2(L"TitleBarRB") };
	m_titleBarRect.left = titleBarLT.x;
	m_titleBarRect.top = titleBarLT.y;
	m_titleBarRect.right = titleBarRB.x;
	m_titleBarRect.bottom = titleBarRB.y;
}

void IWindow::SetNinePatch(const std::shared_ptr<Resource::Property>& prop)
{
	m_ninePatch.fill(nullptr);

	size_t index{ 0 };
	for (const auto& name : { L"lt", L"t", L"rt", L"l", L"c", L"r", L"lb", L"b", L"rb" })
	{
		if (auto sprite{ prop->GetSprite(name) })
			m_ninePatch[index] = sprite;
		++index;
	}
}

void IWindow::UpdateMouseOverControl(int x, int y)
{
	auto rit{ std::ranges::find_if(std::views::reverse(m_controls), [x, y](const auto& control) { return control->Contains({ x, y }); }) };
	if (rit == m_controls.rend())
	{
		if (auto control{ m_mouseOverControl.lock() })
			control->OnMouseLeave(x, y);
		m_mouseOverControl.reset();
		return;
	}

	auto control{ m_mouseOverControl.lock() };
	if (control != *rit)
	{
		if (control)
			control->OnMouseLeave(x, y);
		m_mouseOverControl = *rit;
		(*rit)->OnMouseEnter(x, y);
	}
}

void IWindow::UpdateFocusControl(int x, int y)
{
	// 포커스 컨트롤 갱신
	m_focusControl.reset();
	for (const auto& control : m_controls)
	{
		if (control->IsEnable() && control->Contains({ x, y }))
		{
			control->SetFocus(true);
			m_focusControl = control;
		}
		else
		{
			control->SetFocus(false);
		}
	}
}

void IWindow::RenderNinePatch() const
{
	const auto& [lt, t, rt, l, c, r, lb, b, rb] { m_ninePatch };
	Graphics::D2D::DrawSprite(lt, Float2{});
	Graphics::D2D::DrawSprite(rt, Float2{ m_size.x - rt->GetSize().x, 0.0f });
	Graphics::D2D::DrawSprite(t, RectF{ lt->GetSize().x, 0.0f, m_size.x - rt->GetSize().x, t->GetSize().y });
	Graphics::D2D::DrawSprite(lb, Float2{ 0.0f, m_size.y - lb->GetSize().y });
	Graphics::D2D::DrawSprite(rb, Float2{ m_size.x - rb->GetSize().x, m_size.y - rb->GetSize().y });
	Graphics::D2D::DrawSprite(b, RectF{ lb->GetSize().x, m_size.y - b->GetSize().y, m_size.x - rb->GetSize().x, static_cast<float>(m_size.y) });
	Graphics::D2D::DrawSprite(l, RectF{ 0.0f, lt->GetSize().y, l->GetSize().x, m_size.y - lb->GetSize().y });
	Graphics::D2D::DrawSprite(r, RectF{ m_size.x - r->GetSize().x, rt->GetSize().y, static_cast<float>(m_size.x), m_size.y - rb->GetSize().y });
	Graphics::D2D::DrawSprite(c, RectF{ lt->GetSize().x, lt->GetSize().y, m_size.x - rb->GetSize().x, m_size.y - rb->GetSize().y });
}
