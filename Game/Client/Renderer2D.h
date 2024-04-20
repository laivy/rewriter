#pragma once

namespace Renderer2D
{
	constexpr auto FRAME_COUNT{ 3 };
	extern ComPtr<ID3D11On12Device> d3d11On12Device;
	extern ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	extern ComPtr<ID3D11Resource> wrappedBackBuffers[FRAME_COUNT];
	extern ComPtr<ID2D1DeviceContext2> ctx;
	extern ComPtr<ID2D1Factory3> d2dFactory;
	extern ComPtr<ID2D1Device2> d2dDevice;
	extern ComPtr<ID2D1Bitmap1> d2dRenderTargets[FRAME_COUNT];
	extern ComPtr<IDWriteFactory5> dwriteFactory;

	void Init();
	void RenderStart();
	void RenderEnd();

	// 이미지의 좌측 상단을 position에 위치하도록 그린다.
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity = 1.0f);

	// 이미지를 rect 범위에 맞춰 그린다.
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const RECTI& rect, float opacity = 1.0f);

	void DrawRect(const RECTI& rect);
}