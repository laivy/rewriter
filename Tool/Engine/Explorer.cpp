#include "stdafx.h"
#include "Explorer.h"

Explorer::Explorer() :
	m_path{ std::filesystem::current_path() }
{
}

void Explorer::Render()
{
	if (ImGui::Begin("Explorer"))
	{
		ImGuiWindowFlags windowFlag{ ImGuiWindowFlags_HorizontalScrollbar };
		if (ImGui::BeginChild("ChildL"))
		{
			std::vector<std::string> folders{};
			std::string temp{ m_path.string() };
			while (true)
			{
				size_t pos{ temp.find("\\") };
				if (pos == std::string::npos)
				{
					if (!temp.empty())
						folders.push_back(temp);
					break;
				}

				folders.push_back(temp.substr(0, pos));
				temp = temp.substr(pos + 1);
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{});
			for (int i = 0; i < folders.size(); ++i)
			{
				const auto& f{ folders[i] };
				if (f == folders.front())
				{
					if (ImGui::Button((f + "/").c_str()))
					{
						m_path = std::filesystem::path{ f + "\\" };
					}
					if (folders.size() != 1)
					{
						ImGui::SameLine();
						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 1.0f, 3.0f });
					}
				}
				else if (f == folders.back())
				{
					ImGui::PopStyleVar();
					ImGui::Button(f.c_str());
				}
				else
				{
					if (ImGui::Button((f + "/").c_str()))
					{
						std::string subPath{ folders.front() };
						for (int j = 1; j <= i; ++j)
						{
							subPath = std::format("{}\\{}", subPath, folders[j]);
						}
						m_path = std::filesystem::path{ subPath };
					}
					ImGui::SameLine();
				}
			}
			ImGui::PopStyleVar();
			ImGui::Spacing();
			ImGui::Separator();

			// 현재 디렉토리에 있는 것들
			// 뒤로가기, 폴더, 파일 순서
			if (ImGui::BeginChild("PATH", ImVec2{}, false, ImGuiWindowFlags_HorizontalScrollbar))
			{
				if (m_path.compare(m_path.root_path()) != 0)
				{
					if (ImGui::Button(".."))
					{
						m_path = std::filesystem::canonical(m_path / "..");
					}
				}

				for (const auto& p : std::filesystem::directory_iterator{ m_path }
				| std::views::filter([](const auto& p) { return std::filesystem::is_directory(p); }))
				{
					std::string name{ p.path().filename().string() };
					if (ImGui::Button(name.c_str()))
					{
						m_path = std::filesystem::canonical(m_path / name.c_str());
					}
				}

				for (const auto& p : std::filesystem::directory_iterator{ m_path }
				| std::views::filter([](const auto& p) { return std::filesystem::is_regular_file(p); }))
				{
					std::string name{ p.path().filename().string() };
					ImGui::Selectable(name.c_str());
					if (ImGui::BeginDragDropSource())
					{
						auto fullPath{ p.path().string() };
						ImGui::SetDragDropPayload("FILE_TO_HIERARCHY", fullPath.data(), fullPath.size() + 1);
						ImGui::Text(name.c_str());
						ImGui::EndDragDropSource();
					}
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}
