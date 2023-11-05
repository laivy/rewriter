#pragma once

class Resource::Image;

namespace Renderer2D
{
	extern ComPtr<ID2D1DeviceContext2> g_ctx;

	// 이미지의 좌측 상단을 position에 위치하도록 그린다.
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity = 1.0f);

	// 이미지를 rect 범위에 맞춰 그린다.
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const RECTI& rect, float opacity = 1.0f);

	void DrawRect(const RECTI& rect);
}