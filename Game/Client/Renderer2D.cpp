#include "Stdafx.h"
#include "App.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

namespace Renderer2D
{
	ComPtr<ID2D1DeviceContext2> ctx;
	ComPtr<ID2D1Factory3> d2dFactory;
	ComPtr<ID2D1Device2> d2dDevice;
	ComPtr<ID2D1Bitmap1> d2dRenderTargets[3];
	ComPtr<IDWriteFactory5> dwriteFactory;

	void RenderStart()
	{
		Renderer3D::d3d11On12Device->AcquireWrappedResources(Renderer3D::wrappedBackBuffers[Renderer3D::frameIndex].GetAddressOf(), 1);
		ctx->SetTarget(d2dRenderTargets[Renderer3D::frameIndex].Get());
		ctx->BeginDraw();
	}

	void RenderEnd()
	{
		DX::ThrowIfFailed(ctx->EndDraw());
		Renderer3D::d3d11On12Device->ReleaseWrappedResources(Renderer3D::wrappedBackBuffers[Renderer3D::frameIndex].GetAddressOf(), 1);
		Renderer3D::d3d11DeviceContext->Flush();
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