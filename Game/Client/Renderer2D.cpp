#include "Stdafx.h"
#include "App.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

namespace Renderer2D
{
	ComPtr<ID3D11On12Device> d3d11On12Device;
	ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	ComPtr<ID3D11Resource> wrappedBackBuffers[FRAME_COUNT];
	ComPtr<ID2D1DeviceContext2> ctx;
	ComPtr<ID2D1Factory3> d2dFactory;
	ComPtr<ID2D1Device2> d2dDevice;
	ComPtr<ID2D1Bitmap1> d2dRenderTargets[3];
	ComPtr<IDWriteFactory5> dwriteFactory;

	void Init()
	{
		// D3D11on12 디바이스 생성
		ComPtr<ID3D11Device> d3d11Device;
		DX::ThrowIfFailed(D3D11On12CreateDevice(
			Renderer3D::d3dDevice.Get(),
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr,
			0,
			reinterpret_cast<IUnknown**>(Renderer3D::commandQueue.GetAddressOf()),
			1,
			0,
			&d3d11Device,
			&d3d11DeviceContext,
			nullptr
		));
		DX::ThrowIfFailed(d3d11Device.As(&d3d11On12Device));

		// 팩토리 생성
		DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf()));

		// 디바이스 생성
		ComPtr<IDXGIDevice> dxgiDevice;
		DX::ThrowIfFailed(d3d11On12Device.As(&dxgiDevice));
		DX::ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice));
		DX::ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ctx));
		DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &dwriteFactory));

		// 렌더 타겟 생성
		UINT dpi{ ::GetDpiForWindow(App::GetInstance()->GetHwnd()) };
		D2D1_BITMAP_PROPERTIES1 bitmapProperties{ D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			static_cast<float>(dpi),
			static_cast<float>(dpi)
		) };

		for (size_t i = 0; i < FRAME_COUNT; ++i)
		{
			D3D11_RESOURCE_FLAGS d3d11Flags{ D3D11_BIND_RENDER_TARGET };
			DX::ThrowIfFailed(d3d11On12Device->CreateWrappedResource(
				Renderer3D::renderTargets[i].Get(),
				&d3d11Flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&wrappedBackBuffers[i])
			));

			ComPtr<IDXGISurface> surface;
			DX::ThrowIfFailed(wrappedBackBuffers[i].As(&surface));
			DX::ThrowIfFailed(ctx->CreateBitmapFromDxgiSurface(
				surface.Get(),
				&bitmapProperties,
				&d2dRenderTargets[i]
			));
		}
	}

	void RenderStart()
	{
		d3d11On12Device->AcquireWrappedResources(wrappedBackBuffers[Renderer3D::frameIndex].GetAddressOf(), 1);
		ctx->SetTarget(d2dRenderTargets[Renderer3D::frameIndex].Get());
		ctx->BeginDraw();
	}

	void RenderEnd()
	{
		DX::ThrowIfFailed(ctx->EndDraw());
		d3d11On12Device->ReleaseWrappedResources(wrappedBackBuffers[Renderer3D::frameIndex].GetAddressOf(), 1);
		d3d11DeviceContext->Flush();
	}

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity)
	{
		image->UseAs(ctx, Resource::Image::Type::D2D);
		INT2 size{ image->GetSize() };
		ctx->DrawBitmap(
			static_cast<ID2D1Bitmap*>(image->Get()),
			RECTF{ 
				static_cast<float>(position.x), 
				static_cast<float>(position.y),
				static_cast<float>(position.x + size.x),
				static_cast<float>(position.y + size.y) 
			},
			opacity
		);
	}

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const RECTI& rect, float opacity)
	{
		image->UseAs(ctx, Resource::Image::Type::D2D);
		ctx->DrawBitmap(
			static_cast<ID2D1Bitmap*>(image->Get()),
			RECTF{
				static_cast<float>(rect.left),
				static_cast<float>(rect.top),
				static_cast<float>(rect.right),
				static_cast<float>(rect.bottom)
			},
			opacity
		);
	}

	void DrawRect(const RECTI& rect)
	{
		ComPtr<ID2D1SolidColorBrush> brush{};
		ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Green }, &brush);
		ctx->FillRectangle(
			RECTF{
				static_cast<float>(rect.left),
				static_cast<float>(rect.top),
				static_cast<float>(rect.right),
				static_cast<float>(rect.bottom)
			},
			brush.Get()
		);
	}
}