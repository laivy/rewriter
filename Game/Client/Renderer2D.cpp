#include "Stdafx.h"
#include "ClientApp.h"
#include "Renderer2D.h"

namespace Renderer2D
{
	ComPtr<ID2D1DeviceContext2> g_ctx{};

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity)
	{
		image->UseAs(g_ctx, Resource::Image::Type::D2D);
		INT2 size{ image->GetSize() };
		g_ctx->DrawBitmap(
			image->GetD2DBitmap(),
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
		image->UseAs(g_ctx, Resource::Image::Type::D2D);
		g_ctx->DrawBitmap(
			image->GetD2DBitmap(),
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
		g_ctx->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Green }, &brush);
		g_ctx->FillRectangle(
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