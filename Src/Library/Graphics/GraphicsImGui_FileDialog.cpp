module;

// ImGui
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include "External/ImGui/imgui_stdlib.h"

module rewriter.library.graphics.direct3d:filedialog;

import std;

namespace Graphics::ImGui::FileDialog
{
	struct Context
	{
		bool isOpened{ false };
		std::string_view name;

		std::filesystem::path path; // 현재 탐색중인 폴더 경로
		std::wstring filename; // 선택한 파일 이름
	};

	std::vector<Context> g_contexts;

	void Open(std::string_view str_id)
	{
		Context context{};
		context.name = str_id;
		context.path = std::filesystem::current_path();
		g_contexts.push_back(context);
	}

	bool Begin(std::string_view name, bool* p_open, ImGuiWindowFlags flags)
	{
		auto it{ std::ranges::find_if(g_contexts, [name](const auto& context) { return context.name == name; }) };
		if (it == g_contexts.end())
			return false;

		auto& context{ *it };
		if (!context.isOpened)
		{
			context.isOpened = true;
			::ImGui::OpenPopup(context.name.data());
		}

		::ImGui::SetNextWindowSize(ImVec2{ 700.0f, 400.0f }, ImGuiCond_FirstUseEver);
		if (!::ImGui::BeginPopupModal(context.name.data(), p_open, flags | ImGuiWindowFlags_NoSavedSettings))
		{
			g_contexts.erase(it);
			return false;
		}

		// 주소창
		std::string temp{ context.path.string() };
		::ImGui::SetNextItemWidth(-1.0f);
		if (::ImGui::InputText("##Path", &temp, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (std::filesystem::exists(temp))
				context.path = temp;
			else
				context.path = std::filesystem::current_path();
		}
		::ImGui::Separator();

		// 폴더, 파일 목록
		constexpr auto BottomAreaHeight{ 60.0f };
		if (::ImGui::BeginChild("Folders", ImVec2{ 100.0f, -BottomAreaHeight }, ImGuiChildFlags_ResizeX))
		{
			if (::ImGui::Selectable("..", false, ImGuiSelectableFlags_DontClosePopups))
			{
				if (context.path.has_parent_path())
					context.path = context.path.parent_path();
			}

			for (const auto& entry : std::filesystem::directory_iterator{ context.path })
			{
				if (!entry.is_directory())
					continue;

				if (::ImGui::Selectable(reinterpret_cast<const char*>(entry.path().filename().u8string().c_str()), false, ImGuiSelectableFlags_DontClosePopups))
					context.path = entry.path();
			}
		}
		::ImGui::EndChild();
		::ImGui::SameLine();
		::ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		::ImGui::SameLine();
		if (::ImGui::BeginChild("Files", ImVec2{ 0.0f, -BottomAreaHeight }))
		{
			if (::ImGui::BeginTable("Info", 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_PadOuterX))
			{
				const float CharacterWidth = ::ImGui::CalcTextSize("A").x;

				::ImGui::TableSetupColumn("File");
				::ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, CharacterWidth * 8.0f);
				::ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, CharacterWidth * 5.0f);
				::ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, CharacterWidth * 15.0f);
				::ImGui::TableHeadersRow();
				for (const auto& entry : std::filesystem::directory_iterator{ context.path })
				{
					if (!entry.is_regular_file())
						continue;

					const auto& path{ entry.path() };

					::ImGui::TableNextRow();
					::ImGui::TableNextColumn();
					if (::ImGui::Selectable(path.filename().string().c_str(), path == context.path / context.filename, ImGuiSelectableFlags_SpanAllColumns))
						context.filename = path.filename();

					::ImGui::TableNextColumn();
					::ImGui::Text("%zuKB", std::filesystem::file_size(path) / 1'000);

					::ImGui::TableNextColumn();
					::ImGui::TextUnformatted(path.extension().string().c_str());

					::ImGui::TableNextColumn();
					::ImGui::TextUnformatted(std::format("{0:%F %T}", std::chrono::floor<std::chrono::seconds>(entry.last_write_time())).c_str());
				}
				::ImGui::EndTable();
			}
		}
		::ImGui::EndChild();

		// 하단 영역
		::ImGui::Separator();
		::ImGui::AlignTextToFramePadding();
		::ImGui::TextUnformatted("File Name:");
		::ImGui::SameLine();

		::ImGui::SetNextItemWidth(-1.0f);
		::ImGui::InputText("##File Name", &context.filename);
		if (::ImGui::Button("Open"))
		{
		}
		::ImGui::SameLine();
		if (::ImGui::Button("Close"))
			::ImGui::CloseCurrentPopup();
		return true;
	}

	std::filesystem::path GetPath()
	{
		return L"";
	}
}
