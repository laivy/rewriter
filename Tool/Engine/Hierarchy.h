#pragma once

class Hierarchy :
	public IObserver,
	public TSingleton<Hierarchy>
{
private:
	struct Root
	{
		std::filesystem::path path;
		bool isModified{ false };
	};

	class IModal abstract
	{
	public:
		IModal();
		virtual ~IModal() = default;

		virtual void Run() = 0;

		void Close();
		bool IsValid() const;

	private:
		bool m_isValid;
	};

public:
	Hierarchy();
	~Hierarchy() = default;

	void Update(float deltaTime);
	void Render();

	void OpenTree(const std::shared_ptr<Resource::Property>& prop);
	void CloseTree(const std::shared_ptr<Resource::Property>& prop);
	bool IsRoot(const std::shared_ptr<Resource::Property>& prop) const;

private:
	void OnPropertyAdd(const std::shared_ptr<Resource::Property>& prop);
	void OnPropertyDelete(const std::shared_ptr<Resource::Property>& prop);
	void OnPropertyModified(const std::shared_ptr<Resource::Property>& prop);
	void OnPropertySelected(const std::shared_ptr<Resource::Property>& prop);
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
	void RenderModal();

	void LoadDataFile(const std::filesystem::path& path);
	void Recurse(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func);
	void Add(const std::shared_ptr<Resource::Property>& parent, const std::shared_ptr<Resource::Property>& child);
	void Delete(const std::shared_ptr<Resource::Property>& prop);
	void Save(const std::shared_ptr<Resource::Property>& prop);
	void SetModified(const std::shared_ptr<Resource::Property>& prop, bool modified);

	std::shared_ptr<Resource::Property> GetRoot(const std::shared_ptr<Resource::Property>& prop) const;
	bool IsModified(const std::shared_ptr<Resource::Property>& prop) const;
	bool IsSelected(const std::shared_ptr<Resource::Property>& prop) const;
	bool IsOpened(const std::shared_ptr<Resource::Property>& prop) const;

private:
	static constexpr auto WINDOW_NAME{ "Hierarchy" };
	static constexpr auto MENU_FILE{ "File" };
	static constexpr auto MENU_FILE_NEW{ "New" };
	static constexpr auto MENU_FILE_OPEN{ "Open" };
	static constexpr auto MENU_FILE_SAVE{ "Save" };
	static constexpr auto MENU_FILE_SAVEAS{ "Save As" };
	static constexpr auto DEFAULT_FILE_NAME{ L"File" };
	static constexpr auto DEFAULT_PROPERTY_NAME{ L"Property" };

	std::map<std::shared_ptr<Resource::Property>, Root> m_roots;
	std::vector<std::weak_ptr<Resource::Property>> m_invalids;
	std::vector<std::weak_ptr<Resource::Property>> m_selects;
	std::vector<std::weak_ptr<Resource::Property>> m_opens;
	std::vector<std::unique_ptr<IModal>> m_modals;
};
