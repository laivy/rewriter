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
	void RenderFileViewer();

private:
	static constexpr auto WINDOW_NAME{ "Explorer" };

	std::filesystem::path m_path;
	bool m_scrollAddressBarToRight;
};
