#pragma once

namespace Graphics::ImGui
{
	DLL_API void Begin();
	DLL_API void End();

	DLL_API ImGuiContext* GetContext();

	DLL_API bool Button(std::wstring_view label, const ImVec2& size = ImVec2{ 0, 0 });
}
