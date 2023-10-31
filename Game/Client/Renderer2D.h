#pragma once

class Resource::Image;

namespace Renderer2D
{
	extern ComPtr<ID2D1DeviceContext2> g_ctx;

	// 이미지의 우측 상단을 position에 위치하도록 그린다.
	void DrawImage(Resource::Image* image, const INT2& position = {}, float opacity = 1.0f);
}