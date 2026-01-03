#pragma once

class Explorer : public Singleton<Explorer>
{
private:
	struct FileViewerSelectedItem
	{
		ImGuiID id;
		ImRect rect;
	};

public:
	Explorer();
	~Explorer() = default;

	void Update(float deltaSeconds);
	void Render();

private:
	void FileTree();
	void AddressBar();
	void FileViewer();

	std::vector<std::string> FileViewerSplitString(std::string_view string, const float width);
	bool FileButton(ImTextureID icon, ImVec2 imageSize, std::string_view label, std::wstring dragDropPayload = L"");

	void SetPath(const std::filesystem::path& path);

private:
	std::filesystem::path m_path;
	bool m_addressBarScrollToRight;
	FileViewerSelectedItem m_fileViewerSelectedItem;
};
