#pragma once

class Hierarchy :
	public IObserver,
	public TSingleton<Hierarchy>
{
public:
	Hierarchy();
	~Hierarchy() = default;

	void Update(float deltaTime);
	void Render();

private:
	void OnPropertySelect(std::shared_ptr<Resource::Property> prop);
	void OnMenuFileNew();
	void OnMenuFileOpen();
	void OnMenuFileSave();
	void OnMenuFileSaveAs();
	void OnCut();
	void OnCopy();
	void OnPaste();

	void Shortcut();
	void DragDrop();
	void RenderMenuBar();
	void RenderTreeNode();
	void RenderNode(const std::shared_ptr<Resource::Property>& prop);
	void RenderNodeContextMenu(const std::shared_ptr<Resource::Property>& prop);

	void LoadDataFile(const std::filesystem::path& path);
	void Recurse(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func);
	void Delete(const std::shared_ptr<Resource::Property>& prop);

	std::shared_ptr<Resource::Property> GetParent(const std::shared_ptr<Resource::Property>& prop);
	std::shared_ptr<Resource::Property> GetAncestor(const std::shared_ptr<Resource::Property>& prop);

private:
	static constexpr auto WINDOW_NAME{ "Hierarchy" };
	static constexpr auto MENU_FILE{ "File" };
	static constexpr auto MENU_FILE_NEW{ "New" };
	static constexpr auto MENU_FILE_OPEN{ "Open" };
	static constexpr auto MENU_FILE_SAVE{ "Save" };
	static constexpr auto MENU_FILE_SAVEAS{ "Save as" };
	static constexpr auto DEFAULT_FILE_NAME{ L"NewFile" };
	static constexpr auto DEFAULT_NODE_NAME{ L"NewNode" };

	std::map<std::shared_ptr<Resource::Property>, std::filesystem::path> m_roots;
	std::set<std::shared_ptr<Resource::Property>> m_invalids;
	std::vector<std::weak_ptr<Resource::Property>> m_selects;
};
