#include "Pch.h"
#ifdef _IMGUI
#include "ImGuiFileDialog.h"

namespace
{
	struct Context
	{
		std::string label;
		bool open;
		Graphics::ImGui::ImFileDialog::Type type; // 열기 or 저장
		Graphics::ImGui::ImFileDialog::Target target; // 폴더 or 파일
		std::vector<std::filesystem::path> extensions; // 확장자(타겟이 파일인 경우에만 유효)

		std::filesystem::path path; // 현재 디렉토리 경로
		std::string selected; // 현재 선택된 항목(폴더 or 파일 이름, utf-8 인코딩)
		std::filesystem::path extension; // 현재 선택된 확장자
		std::optional<std::filesystem::path> result;
	};

	std::optional<Context> g_context;

	void AddressBar(Context& ctx)
	{
		if (!ImGui::BeginChild("AddressBar", ImVec2{ 0.0f, ImGui::GetFrameHeight() }))
		{
			ImGui::EndChild();
			return;
		}

		// 디스크 드라이브 콤보 박스
		ImGui::SetNextItemWidth(ImGui::GetFrameHeight() + ImGui::CalcTextSize("AAA").x);
		if (ImGui::BeginCombo("##DiskDrive", reinterpret_cast<const char*>(ctx.path.root_name().string().c_str())))
		{
			DWORD bufferSize{ ::GetLogicalDriveStrings(0, nullptr) };
			std::wstring buffer(bufferSize, L'\0');
			if (!::GetLogicalDriveStrings(bufferSize, buffer.data()))
			{
				ImGui::EndChild();
				return;
			}

			std::vector<std::filesystem::path> drives;
			while (true)
			{
				size_t pos{ buffer.find(L'\0') };
				if (pos == 0 || pos == std::wstring::npos)
					break;

				drives.push_back(buffer.substr(0, pos));
				buffer.erase(0, pos + 1);
			}

			for (const auto& drive : drives)
			{
				if (ImGui::Selectable(reinterpret_cast<const char*>(drive.root_name().string().c_str())))
				{
					ctx.path = drive;
					break;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 3.0f);
		const std::filesystem::path path{ ctx.path };
		std::filesystem::path currentPath;
		for (std::size_t i{ 0 }; const auto& entry : path)
		{
			currentPath /= entry;
			if (entry.has_root_name() || entry.has_root_directory())
				continue;

			ImGui::PushID(i++);

			// 폴더 이름 버튼
			ImGui::SameLine();
			if (ImGui::Button(reinterpret_cast<const char*>(entry.u8string().c_str())))
			{
				ctx.path = currentPath;
				ImGui::PopID();
				break;
			}

			// 하위 폴더가 있으면 ">" 버튼 표시
			std::vector<std::filesystem::path> subFolders;
			for (const auto& subPath : std::filesystem::directory_iterator{ currentPath, std::filesystem::directory_options::skip_permission_denied })
			{
				if (subPath.is_directory())
					subFolders.push_back(subPath);
			}
			if (!subFolders.empty())
			{
				ImGui::SameLine();
				if (ImGui::Button(">"))
					ImGui::OpenPopup("FolderList");
				if (ImGui::BeginPopup("FolderList"))
				{
					for (const auto& subFolder : subFolders)
					{
						if (ImGui::Selectable(reinterpret_cast<const char*>(subFolder.filename().u8string().c_str())))
						{
							ctx.path = subFolder;
							break;
						}
					}
					ImGui::EndPopup();
				}
			}
			ImGui::PopID();
		}
		ImGui::PopStyleVar();

		// 경로가 바뀌지 않았을 때 갱신해야 제대로 작동함
		// if (path.empty() && m_addressBarScrollToRight)
		// {
		//     m_addressBarScrollToRight = false;
		//     ImGui::SetScrollHereX(1.0f);
		// }
		ImGui::EndChild();
	}

	void ItemList(Context& ctx)
	{
		if (!ImGui::BeginChild("ItemList", ImVec2{ 0.0f, -(ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeight()) * 2.0f }))
		{
			ImGui::EndChild();
			return;
		}

		std::vector<std::filesystem::directory_entry> entries;
		for (const auto& entry : std::filesystem::directory_iterator{ ctx.path, std::filesystem::directory_options::skip_permission_denied })
		{
			if (entry.is_directory())
				entries.push_back(entry);
		}
		if (ctx.target == Graphics::ImGui::ImFileDialog::Target::File)
		{
			for (const auto& entry : std::filesystem::directory_iterator{ ctx.path, std::filesystem::directory_options::skip_permission_denied })
			{
				if (!entry.is_regular_file())
					continue;
				if (ctx.extension != L".*" && ctx.extension != entry.path().extension())
					continue;
				entries.push_back(entry);
			}
		}

		if (!ImGui::BeginTable("Items", 4, ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter))
		{
			ImGui::EndChild();
			return;
		}
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("이름", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("확장자", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("가가가").x);
		ImGui::TableSetupColumn("수정한 날짜", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("0000-00-00 00:00").x);
		ImGui::TableSetupColumn("크기", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("000000AA").x);
		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;
		clipper.Begin(static_cast<int>(entries.size()));
		while (clipper.Step())
		{
			for (int i{ clipper.DisplayStart }; i < clipper.DisplayEnd; ++i)
			{
				const auto& entry{ entries.at(i) };
				const auto& path{ entry.path() };
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGuiSelectableFlags flags{ ImGuiSelectableFlags_NoAutoClosePopups | ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick };
				if (path.filename().u8string() == std::u8string{ ctx.selected.begin(), ctx.selected.end() })
					flags |= ImGuiSelectableFlags_Highlight;
				if (ImGui::Selectable(reinterpret_cast<const char*>(path.filename().u8string().c_str()), false, flags))
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (entry.is_directory())
						{
							ctx.path = path;
							ctx.selected.clear();
						}
						else if (entry.is_regular_file() && ctx.target == Graphics::ImGui::ImFileDialog::Target::File)
						{
							ctx.result = path;
						}
					}
					else
					{
						const auto u8string{ path.filename().u8string() };
						ctx.selected.assign(u8string.begin(), u8string.end());
					}
				}

				ImGui::TableNextColumn();
				if (entry.is_regular_file())
					ImGui::TextUnformatted(reinterpret_cast<const char*>(path.extension().u8string().c_str()));

				ImGui::TableNextColumn();
				const auto time{ std::format("{:%F %R}", std::filesystem::last_write_time(path)) };
				ImGui::TextUnformatted(time.c_str());

				if (entry.is_regular_file())
				{
					ImGui::TableNextColumn();
					std::uintmax_t size{ std::max(std::filesystem::file_size(path) / 1024, std::uintmax_t{ 1 }) };
					std::string unit{ "KB" };
					if (size > std::uintmax_t{ 1'000'000 } * 1024)
					{
						size /= std::uintmax_t{ 1024 } * 1024;
						unit = "GB";
					}
					else if (size >= 1'000'000)
					{
						size /= 1024;
						unit = "MB";
					}
					ImGui::Text("%llu%s", size, unit.c_str());
				}
			}
		}
		ImGui::EndTable();
		ImGui::EndChild();
	}

	void Footer(Context& ctx)
	{
		ImGui::SetNextItemWidth(-(ImGui::CalcTextSize(".AAAAA").x + ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.x));
		ImGui::InputText("##selected", &ctx.selected);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::BeginCombo("##extensions", reinterpret_cast<const char*>(ctx.extension.u8string().c_str())))
		{
			for (const auto& ext : ctx.extensions)
			{
				if (ImGui::Selectable(reinterpret_cast<const char*>(ext.u8string().c_str()), ext == ctx.extension))
					ctx.extension = ext;
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("확인"))
		{
			if (ctx.target == Graphics::ImGui::ImFileDialog::Target::Folder && std::filesystem::is_directory(ctx.selected))
			{
				ctx.result = ctx.selected;
			}
			if (ctx.target == Graphics::ImGui::ImFileDialog::Target::File)
			{
				if (ctx.type == Graphics::ImGui::ImFileDialog::Type::Open)
				{
					if (std::filesystem::is_regular_file(ctx.selected))
						ctx.result = ctx.selected;
					else if (std::filesystem::is_directory(ctx.selected))
						ctx.path = ctx.selected;
				}
				else if (ctx.type == Graphics::ImGui::ImFileDialog::Type::Save)
				{
					auto result{ ctx.path / ctx.selected };
					if (ctx.extension != L".*" && ctx.extension != result.extension())
						result += ctx.extension;
					ctx.result = result;
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("취소"))
			ctx.open = false;
	}
}

namespace Graphics::ImGui::ImFileDialog
{
	void Open(const std::string& label, Type type, Target target, const std::vector<std::filesystem::path>& extensions)
	{
		if (g_context)
		{
			assert(false && "이미 파일 다이얼로그가 열려 있습니다.");
			return;
		}
		if (type == Type::Save && target == Target::Folder)
		{
			assert(false && "type, target 조합이 올바르지 않습니다.");
			return;
		}

		Context& ctx{ g_context.emplace() };
		ctx.label = label;
		ctx.open = true;
		ctx.type = type;
		ctx.target = target;
		ctx.extensions = extensions;
		ctx.path = std::filesystem::current_path();
		ctx.extension = extensions.front();
	}

	std::optional<std::filesystem::path> Render(const std::string& label)
	{
		if (!g_context)
			return std::nullopt;
		if (g_context->label != label)
			return std::nullopt;

		::ImGui::OpenPopup(label.c_str());

		auto& ctx{ *g_context };
		::ImGui::SetNextWindowSize(::ImGui::GetWindowViewport()->Size * 0.5f, ImGuiCond_Appearing);
		if (::ImGui::BeginPopupModal(label.c_str(), &ctx.open, ImGuiWindowFlags_NoSavedSettings))
		{
			AddressBar(ctx);
			ItemList(ctx);
			Footer(ctx);
			::ImGui::EndPopup();
		}

		const auto result{ ctx.result };
		if (ctx.result)
			ctx.open = false;
		if (!ctx.open)
			g_context.reset();
		return result;
	}
}
#endif
