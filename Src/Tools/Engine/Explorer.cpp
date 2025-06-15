#include "Stdafx.h"
#include "Explorer.h"
#include "Common/Util.h"

namespace
{
	std::vector<std::string> SplitString(std::string_view string, const float width)
	{
		std::vector<std::string> lines;
		size_t i{ 1 };
		while (!string.empty())
		{
			std::string_view substr{ string.substr(0, i) };
			if (ImGui::CalcTextSize(substr.data(), substr.data() + i).x > width)
			{
				std::string_view line{ string.substr(0, i - 1) };
				lines.emplace_back(line);
				string = string.substr(i - 1);

				// 3줄까지만 표시
				if (lines.size() >= 2)
				{
					if (ImGui::CalcTextSize(string.data(), string.data() + string.size()).x <= width)
					{
						lines.emplace_back(string);
						return lines;
					}

					// 남은 문자열의 길이가 width를 넘으면 말줄임 처리
					const ImVec2 ellipsisTextSize{ ImGui::CalcTextSize("...") };
					size_t j{ string.size() };
					while (true)
					{
						std::string ellipsized{ string.substr(0, j) };
						if (ImGui::CalcTextSize(ellipsized.data(), ellipsized.data() + ellipsized.size()).x + ellipsisTextSize.x <= width)
						{
							ellipsized += "...";
							lines.emplace_back(std::move(ellipsized));
							break;
						}
						if (j == 0)
						{
							break;
						}
						--j;
					}
					return lines;
				}

				i = 1;
				continue;
			}
			if (i == string.size())
			{
				lines.emplace_back(string);
				break;
			}
			++i;
		}

		return lines;
	}

	bool IconButton(const std::shared_ptr<Graphics::ImGui::Texture>& icon, std::string_view label)
	{
		// wrapWidth를 기준으로 줄로 나눔
		constexpr float wrapWidth{ 80.0f };
		const std::vector<std::string> lines{ SplitString(label, wrapWidth) };

		// 너비는 고정, 높이는 가변
		constexpr float itemWidth{ 100.0f };
		constexpr ImVec2 iconSize{ 60.0f, 60.0f };
		const ImVec2 itemSpacing{ ImGui::GetStyle().ItemSpacing };
		const float fontHeight{ ImGui::GetFontSize() };
		const ImVec2 itemSize{ itemWidth, iconSize.y + itemSpacing.y * (lines.size() + 1) + fontHeight * lines.size() };

		// 줄바꿈
		if (ImGui::GetContentRegionMax().x < ImGui::GetCursorPosX() + itemSpacing.x + itemSize.x)
			ImGui::Spacing();

		// 선택된 아이템 하이라이트
		static ImGuiID selectedItemID{ 0 };
		if (selectedItemID == ImGui::GetID(label.data()))
		{
			const ImVec2 startCursorScreenPos{ ImGui::GetCursorScreenPos() };
			const ImVec2 lt{ startCursorScreenPos };
			const ImVec2 rb{ startCursorScreenPos.x + itemSize.x, startCursorScreenPos.y + itemSize.y };
			ImDrawList* drawList{ ImGui::GetWindowDrawList() };
			drawList->AddRectFilled(lt, rb, IM_COL32(80, 80, 80, 255));
			drawList->AddRect(lt, rb, IM_COL32(195, 195, 195, 255));
		}

		ImGui::BeginGroup();

		// 아이콘
		const ImVec2 startCursorPos{ ImGui::GetCursorPos() };
		ImGui::SetCursorPosX(startCursorPos.x + (itemWidth - iconSize.x) / 2.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemSpacing.y);
		Graphics::ImGui::Image(icon, iconSize);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - itemSpacing.y);

		// 파일 또는 폴더 이름
		for (const auto& line : lines)
		{
			ImVec2 textSize{ ImGui::CalcTextSize(line.data(), line.data() + line.size()) };
			ImGui::SetCursorPosX(startCursorPos.x + (itemWidth - textSize.x) / 2.0f);
			ImGui::TextUnformatted(line.data(), line.data() + line.size());
		}

		// 전체를 덮는 버튼
		bool isClicked{ false };
		ImGui::SetCursorPos(startCursorPos);
		if (ImGui::InvisibleButton(label.data(), itemSize))
		{
			ImGuiID itemID{ ImGui::GetItemID() };
			if (selectedItemID == itemID)
			{
				selectedItemID = 0;
				isClicked = true;
			}
			else
			{
				selectedItemID = itemID;
			}
		}

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

		static const auto icon{ Graphics::ImGui::LoadTexture(L"Engine/Icon/Folder.png") };
		std::wstring name{ entry.path().filename() };
		if (IconButton(icon, Util::wstou8s(name)))
			SetPath(std::filesystem::canonical(m_path / name));
		ImGui::SameLine();
	}

	// 파일
	for (const auto& entry : std::filesystem::directory_iterator{ m_path })
	{
		if (!entry.is_regular_file() || entry.path().extension() != Stringtable::DATA_FILE_EXT)
			continue;

		static const auto icon{ Graphics::ImGui::LoadTexture(L"Engine/Icon/File.png") };
		std::string name{ Util::u8stou8s(entry.path().filename().u8string()) };
		IconButton(icon, name);
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
