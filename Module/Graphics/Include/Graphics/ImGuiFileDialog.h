#pragma once
#ifdef _IMGUI
// C++ 표준 라이브러리
#include <filesystem>
#include <optional>
#include <vector>
#include <string>

// 프로젝트
#include "Export.h"

namespace Graphics::ImGui::FileDialog
{
	enum class Type
	{
		Open,
		Save
	};

	enum class Target
	{
		Folder,
		File,
	};

	GRAPHICS_API void Open(const std::string& label, Type type = Type::Open, Target target = Target::File, const std::vector<std::filesystem::path>& extensions = { L".*" });
	GRAPHICS_API std::optional<std::filesystem::path> Render(const std::string& label);
}
#endif
