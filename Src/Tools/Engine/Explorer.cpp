#include "Stdafx.h"
#include "Explorer.h"
#include "Common/Util.h"

namespace
{
	bool Button(std::string_view label)
	{
		static const auto folderIcon{ Graphics::ImGui::LoadTexture(L"Engine/Icon/Folder.png") };

		constexpr ImVec2 iconSize{ 60.0f, 60.0f };
		const ImVec2 textSize{ ImGui::CalcTextSize(label.data()) };

		if (ImGui::GetWindowWidth() < ImGui::GetCursorPosX() + std::max(iconSize.x, textSize.x))
			ImGui::Spacing();

		bool isClicked{ false };
		float iconX{};
		float textX{};
		if (iconSize.x < textSize.x)
		{
			iconX = ImGui::GetCursorPosX() + (textSize.x - iconSize.x) / 2.0f;
			textX = ImGui::GetCursorPosX();
		}
		else
		{
			iconX = ImGui::GetCursorPosX();
			textX = ImGui::GetCursorPosX() + (iconSize.x - textSize.x) / 2.0f;
		}

		static ImGuiID selectedItemID{ 0 };
		ImGuiID id{ ImGui::GetID(label.data()) };

		ImGui::BeginGroup();
		ImGui::SetCursorPosX(iconX);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ ImGui::GetStyle().ItemSpacing.x, -3.0f });
		Graphics::ImGui::Image(folderIcon, iconSize);
		ImGui::PopStyleVar();
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			if (selectedItemID == id)
			{
				isClicked = true;
				selectedItemID = 0;
			}
			else
			{
				selectedItemID = id;
			}
		}
		ImGui::SetCursorPosX(textX);
		if (selectedItemID == id)
		{
			const ImVec2 pos{ ImGui::GetCursorScreenPos() };
			const ImVec2 itemSpacing{ ImGui::GetStyle().ItemSpacing };
			const ImVec2 lt{
				pos.x - itemSpacing.x / 2.0f,
				pos.y - itemSpacing.y / 2.0f
			};
			const ImVec2 rb{
				pos.x + textSize.x + itemSpacing.x / 2.0f,
				pos.y + textSize.y + itemSpacing.y / 2.0f
			};

			ImDrawList* drawList{ ImGui::GetWindowDrawList() };
			drawList->AddRectFilled(lt, rb, IM_COL32(80, 80, 80, 255));
			drawList->AddRect(lt, rb, IM_COL32(195, 195, 195, 255));
		}
		ImGui::TextUnformatted(label.data());
		ImGui::EndGroup();
		return isClicked;
	}
}

Explorer::Explorer() :
	m_scrollAddressBarToRight{ false }
{
	SetPath(std::filesystem::current_path());
}

void Explorer::Update(float deltaTime)
{
}

void Explorer::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME))
	{
		if ( ImGui::BeginChild("ADDRESS_BAR", ImVec2{ 0, 23 }))
			RenderAddressBar();
		ImGui::EndChild();

		ImGui::Separator();

		if (ImGui::BeginChild("FILE_VIEWER", ImVec2{}, false, ImGuiWindowFlags_HorizontalScrollbar) )
			RenderFileViewer();
		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopID();
}

void Explorer::RenderAddressBar()
{
	ImGui::SetNextItemWidth(50.0f);
	if (ImGui::BeginCombo("##DISKDRIVE", reinterpret_cast<const char*>(m_path.root_name().u8string().c_str())))
	{
		DWORD bufferSize{ ::GetLogicalDriveStrings(0, nullptr) };
		std::wstring buffer(bufferSize, L'\0');
		::GetLogicalDriveStrings(bufferSize, buffer.data());

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
			if (ImGui::Selectable(reinterpret_cast<const char*>(drive.root_name().u8string().c_str())))
			{
				SetPath(drive);
				break;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 3, 0 });
	std::filesystem::path newPath;
	std::filesystem::path temp{ m_path.root_path() };
	for (const auto& entry : m_path)
	{
		if (entry.has_root_name() || entry.has_root_directory())
			continue;

		temp /= entry;

		ImGui::SameLine();
		if (ImGui::Button(entry))
		{
			newPath = temp;
			break;
		}
	}
	if (!newPath.empty())
		SetPath(newPath);
	ImGui::PopStyleVar();

	if (m_scrollAddressBarToRight)
	{
		ImGui::SetScrollHereX(1.0f);
		m_scrollAddressBarToRight = false;
	}
}

void Explorer::RenderFileViewer()
{
	// 폴더
	for (const auto& entry : std::filesystem::directory_iterator{ m_path })
	{
		if (!entry.is_directory())
			continue;

		std::wstring name{ entry.path().filename() };
		ImGui::SameLine();
		if (Button(Util::wstou8s(name)))
		//if (ImGui::Button(name))
			SetPath(std::filesystem::canonical(m_path / name));
	}

	// 파일
	for (const auto& entry : std::filesystem::directory_iterator{ m_path })
	{
		if (!entry.is_regular_file() || entry.path().extension() != Stringtable::DATA_FILE_EXT)
			continue;

		std::string name{ Util::u8stou8s(entry.path().filename().u8string()) };
		ImGui::Selectable(name);
		if (ImGui::BeginDragDropSource())
		{
			std::wstring fullPath{ entry.path() };
			fullPath.push_back(L'\0');
			ImGui::SetDragDropPayload("EXPLORER/OPENFILE", fullPath.data(), fullPath.size() * sizeof(std::wstring::value_type));
			ImGui::Text(name.c_str());
			ImGui::EndDragDropSource();
		}
	}
}

void Explorer::SetPath(const std::filesystem::path& path)
{
	m_path = path;
	m_scrollAddressBarToRight = true;
}
