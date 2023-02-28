#include "Stdafx.h"
#include "BrushPool.h"
#include "NytApp.h"

BrushPool::BrushPool()
{
	auto ctx{ NytApp::GetInstance()->GetD2DContext() };
	ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &m_pool[static_cast<int>(BrushType::BLACK)]);
	ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::White }, &m_pool[static_cast<int>(BrushType::WHITE)]);
	ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Red }, &m_pool[static_cast<int>(BrushType::RED)]);
	ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Green }, &m_pool[static_cast<int>(BrushType::GREEN)]);
	ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Blue }, &m_pool[static_cast<int>(BrushType::BLUE)]);
}

ComPtr<ID2D1SolidColorBrush> BrushPool::GetBrush(BrushType type)
{
	int key{ static_cast<int>(type) };
	if (m_pool.contains(key))
		return m_pool.at(key);
	return m_pool[static_cast<int>(BrushType::BLACK)];
}
