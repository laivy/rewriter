#include "stdafx.h"
#include "ProjectWindow.h"

ProjectWindow::ProjectWindow() :
	m_path{ std::filesystem::current_path() }
{
}

void ProjectWindow::Render()
{
	if (ImGui::Begin("Project"))
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
			for (const auto& f : folders)
			{
				if (f == folders.front())
				{
					ImGui::Button((f + "/").c_str());
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
					ImGui::Button((f + "/").c_str());
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
