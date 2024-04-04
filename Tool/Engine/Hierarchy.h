#pragma once

struct PropInfo;

class Hierarchy : public TSingleton<Hierarchy>
{
public:
	Hierarchy();
	~Hierarchy();

	void Update(float deltaTime);
	void Render();

private:
	void OnNodeSelect(std::shared_ptr<Resource::Property> prop);
	void OnFileDragDrop(std::string_view path);

	// 각 메뉴 선택 시 호출된다.
	void OnMenuFileNew();
	void OnMenuFileOpen();
	void OnMenuFileSave();
	void OnMenuFileSaveAs();

	void ProcessDragDrop();
	void RenderMenu();
	void RenderNode();
	void DeleteInvalidNodes();

private:
	static constexpr auto WINDOW_NAME{ "Hierarchy" };
	static constexpr auto MENU_FILE{ "File" };
	static constexpr auto MENU_FILE_NEW{ "New" };
	static constexpr auto MENU_FILE_OPEN{ "Open" };
	static constexpr auto MENU_FILE_SAVE{ "Save" };
	static constexpr auto MENU_FILE_SAVEAS{ "Save as" };
	static constexpr auto DEFAULT_FILE_NAME{ L"NewFile" };

	Observer<std::shared_ptr<Resource::Property>> m_onNodeSelect;

	std::vector<std::shared_ptr<Resource::Property>> m_roots;
	std::map<Resource::Property*, PropInfo> m_info;
};