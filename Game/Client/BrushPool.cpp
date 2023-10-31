#include "Stdafx.h"
#include "BrushPool.h"
#include "ClientApp.h"

BrushPool::BrushPool()
{
	//auto ctx{ ClientApp::GetInstance()->GetD2DContext() };
	//ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &m_pool[BLACK]);
	//ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::White }, &m_pool[WHITE]);
	//ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Red }, &m_pool[RED]);
	//ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Green }, &m_pool[GREEN]);
	//ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Blue }, &m_pool[BLUE]);
}

ID2D1SolidColorBrush* BrushPool::GetBrush(Type type) const
{
	int key{ static_cast<int>(type) };
	if (m_pool.contains(key))
		return m_pool.at(key).Get();
	return m_pool.at(type).Get();
}