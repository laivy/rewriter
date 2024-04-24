#include "Stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"

namespace Renderer2D
{
	void Begin()
	{
		Renderer::d3d11On12Device->AcquireWrappedResources(Renderer::wrappedBackBuffers[Renderer::frameIndex].GetAddressOf(), 1);
		Renderer::ctx->SetTarget(Renderer::d2dRenderTargets[Renderer::frameIndex].Get());
		Renderer::ctx->BeginDraw();
	}

	void End()
	{
		DX::ThrowIfFailed(Renderer::ctx->EndDraw());
		Renderer::d3d11On12Device->ReleaseWrappedResources(Renderer::wrappedBackBuffers[Renderer::frameIndex].GetAddressOf(), 1);
		Renderer::d3d11DeviceContext->Flush();
	}

	void DrawRect(const RECTI& rect)
	{
		ComPtr<ID2D1SolidColorBrush> brush{};
		Renderer::ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Green }, &brush);
		Renderer::ctx->FillRectangle(
			RECTF{
				static_cast<float>(rect.left),
				static_cast<float>(rect.top),
				static_cast<float>(rect.right),
				static_cast<float>(rect.bottom)
			},
			brush.Get()
		);
	}

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity)
	{
		image->Use(Renderer::ctx);
		INT2 size{ image->GetSize() };

		RECTI rect{ 0, 0, size.x, size.y };
		rect.Offset(position.x, position.y);
		
		DrawImage(image, rect, opacity);
	}

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const RECTI& rect, float opacity)
	{
		image->Use(Renderer::ctx);
		Renderer::ctx->DrawBitmap(
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
}