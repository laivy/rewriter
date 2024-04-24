#pragma once

namespace Renderer3D
{
	void Begin();
	void End();

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position = {}, float opacity = 1.0f);
}