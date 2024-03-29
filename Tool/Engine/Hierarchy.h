#pragma once

class Node;

class Hierarchy : public TSingleton<Hierarchy>
{
public:
	Hierarchy();
	~Hierarchy();

	void Update(float deltaTime);
	void Render();

private:
	// 노드가 선택될 때 선택된 노드를 전달받는다.
	bool OnNodeSelect(Node* node);

	// 프로젝트 윈도우로부터 드래드 드랍으로 파일 절대 경로를 전달받는다.
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
	static constexpr auto DEFAULT_FILE_NAME{ "NewFile" };

	std::vector<std::unique_ptr<Node>> m_roots;

	Observer<Node*> m_onNodeSelect;
};