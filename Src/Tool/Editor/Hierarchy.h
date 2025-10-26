#pragma once

class Hierarchy :
	public IDelegate::IListener,
	public TSingleton<Hierarchy>
{
private:
	struct Root
	{
		Resource::ID id;
		std::filesystem::path filePath;
	};

	struct Context
	{
		bool isInvalid;
		bool isModified;
		bool isOpened;
		bool isSelected;
		bool openRenamePopup;
	};

public:
	Hierarchy();
	~Hierarchy() = default;

	void Update(float deltaTime);
	void Render();

	void OpenTree(Resource::ID id);
	void CloseTree(Resource::ID id);
	bool IsRoot(Resource::ID id) const;

private:
	// 델리게이트
	void OnPropertyAdded(Resource::ID id);
	void OnPropertyDeleted(Resource::ID id);
	void OnPropertyModified(Resource::ID id);
	void OnPropertySelected(Resource::ID id);

	// 메뉴
	void OnMenuFileNew();
	void OnMenuFileOpen();
	void OnMenuFileSave();
	void OnMenuFileSaveAs();

	// 단축키
	void OnCut();
	void OnCopy();
	void OnPaste();

	void Shortcut();
	void DragDrop();
	void MenuBar();
	void TreeView();
	void RenderModal();

	void LoadFromFile(const std::filesystem::path& filePath);
	Resource::ID New(Resource::ID parentID);
	void Delete(Resource::ID id);
	void SetModified(Resource::ID id, bool modified);
	void SetRenamePopup(Resource::ID id, bool opened);

	Root* GetRoot(Resource::ID id);
	bool IsModified(Resource::ID id) const;
	bool IsOpened(Resource::ID id) const;
	bool IsSelected(Resource::ID id) const;
	bool IsRenamePopupOpened(Resource::ID id) const;

private:
	std::vector<Root> m_roots;
	std::unordered_map<Resource::ID, Context> m_contexts;

	bool m_isAnyPropertySelected;
};
