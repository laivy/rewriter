#include "Stdafx.h"
#include "Explorer.h"
#include "Util.h"

Explorer::Explorer()
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
		RenderAddressBar();
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::BeginChild(CHILD_WINDOW_NAME, {}, false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			RenderFileView();
		}
		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopID();
}

void Explorer::SetPath(const std::filesystem::path& path)
{
	m_path = path;
	m_folders.clear();
	for (const auto& p : m_path)
	{
		if (p.has_root_directory())
			continue;
		if (p.has_root_name())
		{
			m_folders.push_back(p.string() + "/");
			continue;
		}
		m_folders.push_back(p.string());
	}
}

void Explorer::RenderAddressBar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{});

	std::string path{};
	for (const auto& folder : m_folders)
	{
		if (path.empty())
			path += folder;
		else
			path += "/" + folder;

		ImVec2 val{ 0, 3 };
		if (folder == m_folders.front() || folder == m_folders.back())
			val.x = 4;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, val);

		if (ImGui::Button((folder + "/").c_str()))
			SetPath(path);

		ImGui::PopStyleVar();
		ImGui::SameLine();
	}

	ImGui::NewLine();
	ImGui::PopStyleVar();
}

void Explorer::RenderFileView()
{
	// 현재 디렉토리에 있는 것들
	// 뒤로가기, 폴더, 파일 순서
	if (m_path.compare(m_path.root_path()) != 0)
	{
		if (ImGui::Button(".."))
		{
			SetPath(std::filesystem::canonical(m_path / ".."));
		}
	}

	for (const auto& d : std::filesystem::directory_iterator{ m_path }
					   | std::views::filter([](const auto& d) { return d.is_directory(); }))
	{
		std::wstring name{ d.path().filename().wstring() };
		if (ImGui::Button(Util::wstou8s(name).c_str()))
			SetPath(std::filesystem::canonical(m_path / name.c_str()));
	}

	for (const auto& d : std::filesystem::directory_iterator{ m_path }
					   | std::views::filter([](const auto& d) { return d.is_regular_file(); }))
	{
		std::string name{ Util::wstou8s(d.path().filename().wstring()) };
		ImGui::Selectable(name.c_str());
		if (ImGui::BeginDragDropSource())
		{
			auto fullPath{ d.path().string() };
			ImGui::SetDragDropPayload("FILE_TO_HIERARCHY", fullPath.data(), fullPath.size() + 1);
			ImGui::Text(name.c_str());
			ImGui::EndDragDropSource();
		}
	}
}
