#include "Stdafx.h"
#include "LoginWnd.h"
#include "Renderer2D.h"

LoginWnd::LoginWnd(const INT2& size) : Wnd{ size }
{
}

void LoginWnd::Update(float deltaTime)
{
	__super::Update(deltaTime);
}

void LoginWnd::Render() const
{
	auto patch{ Resource::Get("UI.dat/Login/9Patch") };
	auto lt{ Resource::GetImage(patch, "LT") };
	auto t{ Resource::GetImage(patch, "T") };
	auto rt{ Resource::GetImage(patch, "RT") };
	auto l{ Resource::GetImage(patch, "L") };
	auto r{ Resource::GetImage(patch, "R") };
	auto lb{ Resource::GetImage(patch, "LB") };
	auto b{ Resource::GetImage(patch, "B") };
	auto rb{ Resource::GetImage(patch, "RB") };

	Renderer2D::DrawRect(RECTI{ 0, 0, m_size.x, m_size.y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(lt, INT2{ 0, 0 } + m_position);
	Renderer2D::DrawImage(t, RECTI{ Resource::GetSize(lt).x, 0, m_size.x - Resource::GetSize(rt).y, Resource::GetSize(t).y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(rt, INT2{ m_size.x - Resource::GetSize(rt).x } + m_position);
	Renderer2D::DrawImage(l, RECTI{ 0, Resource::GetSize(lt).y, Resource::GetSize(l).x, m_size.y - Resource::GetSize(lb).y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(r, RECTI{ m_size.x - Resource::GetSize(r).x, Resource::GetSize(rt).y, m_size.x, m_size.y - Resource::GetSize(rb).y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(lb, INT2{ 0, m_size.y - Resource::GetSize(lb).y } + m_position);
	Renderer2D::DrawImage(b, RECTI{ Resource::GetSize(lb).x, m_size.y - Resource::GetSize(b).y, m_size.x - Resource::GetSize(rb).y, m_size.y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(rb, INT2{ m_size.x - Resource::GetSize(rb).x, m_size.y - Resource::GetSize(rb).y } + m_position);
}