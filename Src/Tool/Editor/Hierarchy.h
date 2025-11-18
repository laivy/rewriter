#pragma once

class Hierarchy :
	public IDelegate::Listener,
	public Singleton<Hierarchy>
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
		bool openWarningModal;
	};

public:
	Hierarchy();
	~Hierarchy() = default;

	void Update(float deltaSeconds);
	void Render();

	void Delete(Resource::ID id);
	void SetModified(Resource::ID id, bool modified);
	void SetOpened(Resource::ID id, bool opened);
	void SetSelected(Resource::ID id, bool selected);
	Root* GetRoot(Resource::ID id);
	bool IsRoot(Resource::ID id) const;
	bool IsModified(Resource::ID id) const;
	bool IsOpened(Resource::ID id) const;
	bool IsSelected(Resource::ID id) const;

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
	void OnMenuFileSaveAll();

	// 단축키
	void OnCut();
	void OnCopy();
	void OnPaste();

	void Shortcut();
	void DragDrop();
	void MenuBar();
	void TreeView();
	void FileDialog();

	void LoadFromFile(const std::filesystem::path& filePath);
	Resource::ID New(Resource::ID parentID);

	void SetWarningModal(Resource::ID id, bool opened);
	bool IsWarningModalOpened(Resource::ID id) const;

private:
	std::vector<Root> m_roots;
	std::vector<Resource::ID> m_selectedIDs;
	std::unordered_map<Resource::ID, Context> m_contexts;

	bool m_isAnyPropertySelected;
};
