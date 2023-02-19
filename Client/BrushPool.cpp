#include "Stdafx.h"
#include "BrushPool.h"

BrushPool::BrushPool(const ComPtr<ID2D1DeviceContext2>& renderTarget)
{
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &m_pool[static_cast<int>(BrushType::BLACK)]);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::White }, &m_pool[static_cast<int>(BrushType::WHITE)]);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Red }, &m_pool[static_cast<int>(BrushType::RED)]);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Green }, &m_pool[static_cast<int>(BrushType::GREEN)]);
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Blue }, &m_pool[static_cast<int>(BrushType::BLUE)]);
}

ComPtr<ID2D1SolidColorBrush> BrushPool::GetBrush(BrushType type)
{
	int key{ static_cast<int>(type) };
	if (m_pool.contains(key))
		return m_pool.at(key);
	return m_pool[static_cast<int>(BrushType::BLACK)];
}
