#include "Stdafx.h"
#include "ClientApp.h"
#include "Renderer2D.h"

namespace Renderer2D
{
	ComPtr<ID2D1DeviceContext2> g_ctx{};

	void DrawImage(Resource::Image* image, const INT2& position, float opacity)
	{
		Resource::UseAsD2D(g_ctx, image);
		FLOAT2 size{ Resource::GetSize(image) };
		g_ctx->DrawBitmap(
			static_cast<ID2D1Bitmap*>(*image),
			RECTF{ 
				static_cast<float>(position.x), 
				static_cast<float>(position.y),
				position.x + size.x,
				position.y + size.y 
			},
			opacity
		);
	}
}