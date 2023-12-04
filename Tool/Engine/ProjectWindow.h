#pragma once

class ProjectWindow : public TSingleton<ProjectWindow>
{
public:
	ProjectWindow();
	~ProjectWindow() = default;

	void Render();

private:
	std::filesystem::path m_path;
};