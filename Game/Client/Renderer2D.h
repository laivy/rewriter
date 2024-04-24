#pragma once

namespace Renderer2D
{
	void Begin();
	void End();

	void DrawRect(const RECTI& rect);
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity = 1.0f);
	void DrawImage(const std::shared_ptr<Resource::Image>& image, const RECTI& rect, float opacity = 1.0f);
}