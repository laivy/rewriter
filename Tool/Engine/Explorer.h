#pragma once

class Explorer : public TSingleton<Explorer>
{
public:
	Explorer();
	~Explorer() = default;

	void Update(float deltaTime);
	void Render();

private:
	void SetPath(const std::filesystem::path& path);
	void RenderAddressBar();
	void RenderFileView();

private:
	static constexpr auto WINDOW_NAME{ "Explorer" };
	static constexpr auto CHILD_WINDOW_NAME{ "FileViewer" };

	std::filesystem::path m_path;
	std::vector<std::wstring> m_folders;
};