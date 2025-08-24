#pragma once

class Hierarchy :
	public IObserver,
	public TSingleton<Hierarchy>
{
private:
	struct Root
	{
		Resource::Property::ID id;
		std::filesystem::path path;
	};

	struct Context
	{
		bool isInvalid;
		bool isModified;
		bool isOpened;
		bool isSelected;
	};

public:
	Hierarchy();
	~Hierarchy() = default;

	void Update(float deltaTime);
	void Render();

	void OpenTree(const Resource::Property::ID id);
	void CloseTree(const Resource::Property::ID id);
	bool IsRoot(const Resource::Property::ID id) const;

private:
	// 델리게이트
	void OnPropertyAdded(const Resource::Property::ID id);
	void OnPropertyDeleted(const Resource::Property::ID id);
	void OnPropertyModified(const Resource::Property::ID id);
	void OnPropertySelected(const Resource::Property::ID id);

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
	void RenderMenuBar();
	void RenderPropertyTree();
	void RenderProperty(const Resource::Property::ID id);
	void RenderNodeContextMenu(const Resource::Property::ID id);
	void RenderModal();

	void LoadDataFile(const std::filesystem::path& path);
	void Add(const Resource::Property::ID parentID, const Resource::Property::ID childID);
	void Delete(const Resource::Property::ID id);
	void SetModified(const Resource::Property::ID id, bool modified);

	Root GetRoot(const Resource::Property::ID id) const;
	bool IsModified(const Resource::Property::ID id) const;
	bool IsOpened(const Resource::Property::ID id) const;
	bool IsSelected(const Resource::Property::ID id) const;

private:
	std::vector<Root> m_roots;
	std::unordered_map<Resource::Property::ID, Context> m_contexts;
};
