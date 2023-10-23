#pragma once

class Resource::Property;

namespace Renderer2D
{
	extern ComPtr<ID2D1DeviceContext2> g_ctx;

	void DrawImage(const std::shared_ptr<Resource::Property>& prop, const INT2& position = {}, float opacity = 1.0f);
}