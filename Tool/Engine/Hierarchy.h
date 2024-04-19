#pragma once

class Hierarchy : public TSingleton<Hierarchy>
{
public:
	Hierarchy();
	~Hierarchy();

	void Update(float deltaTime);
	void Render();

private:
	void OnPropertySelect(std::shared_ptr<Resource::Property> prop);
	void OnFileDragDrop(std::string_view path);
	void OnMenuFileNew();
	void OnMenuFileOpen();
	void OnMenuFileSave();
	void OnMenuFileSaveAs();

	void DragDrop();
	void RenderMenu();
	void RenderNode();

private:
	static constexpr auto WINDOW_NAME{ "Hierarchy" };
	static constexpr auto MENU_FILE{ "File" };
	static constexpr auto MENU_FILE_NEW{ "New" };
	static constexpr auto MENU_FILE_OPEN{ "Open" };
	static constexpr auto MENU_FILE_SAVE{ "Save" };
	static constexpr auto MENU_FILE_SAVEAS{ "Save as" };
	static constexpr auto DEFAULT_FILE_NAME{ L"NewFile" };
	static constexpr auto DEFAULT_NODE_NAME{ L"NewNode" };

	Observer<std::shared_ptr<Resource::Property>> m_onNodeSelect;
};