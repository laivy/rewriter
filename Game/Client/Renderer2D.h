#pragma once

namespace Renderer2D
{
	constexpr auto FRAME_COUNT{ 3 };
	extern ComPtr<ID2D1DeviceContext2> ctx;
	extern ComPtr<ID2D1Factory3> d2dFactory;
	extern ComPtr<ID2D1Device2> d2dDevice;
	extern ComPtr<ID2D1Bitmap1> d2dRenderTargets[FRAME_COUNT];
	extern ComPtr<IDWriteFactory5> dwriteFactory;

	void RenderStart();
	void RenderEnd();

	// 이미지의 좌측 상단을 position에 위치하도록 그린다.
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity = 1.0f);

	// 이미지를 rect 범위에 맞춰 그린다.
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const RECTI& rect, float opacity = 1.0f);

	void DrawRect(const RECTI& rect);
}