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
	return;

	auto patch{ Resource::Get(L"UI.dat/Login/9Patch") };
	auto lt{ patch->GetImage(L"LT") };
	auto t{ patch->GetImage(L"T") };
	auto rt{ patch->GetImage(L"RT") };
	auto l{ patch->GetImage(L"L") };
	auto r{ patch->GetImage(L"R") };
	auto lb{ patch->GetImage(L"LB") };
	auto b{ patch->GetImage(L"B") };
	auto rb{ patch->GetImage(L"RB") };

	Renderer2D::DrawRect(RECTI{ 0, 0, m_size.x, m_size.y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(lt, INT2{ 0, 0 } + m_position);
	Renderer2D::DrawImage(t, RECTI{ lt->GetSize().x, 0, m_size.x - rt->GetSize().y, t->GetSize().y}.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(rt, INT2{ m_size.x - rt->GetSize().x } + m_position);
	Renderer2D::DrawImage(l, RECTI{ 0, lt->GetSize().y, l->GetSize().x, m_size.y - lb->GetSize().y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(r, RECTI{ m_size.x - r->GetSize().x, rt->GetSize().y, m_size.x, m_size.y - rb->GetSize().y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(lb, INT2{ 0, m_size.y - lb->GetSize().y } + m_position);
	Renderer2D::DrawImage(b, RECTI{ lb->GetSize().x, m_size.y - b->GetSize().y, m_size.x - rb->GetSize().y, m_size.y }.Offset(m_position.x, m_position.y));
	Renderer2D::DrawImage(rb, INT2{ m_size.x - rb->GetSize().x, m_size.y - rb->GetSize().y } + m_position);
}