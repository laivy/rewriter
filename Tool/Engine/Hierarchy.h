#pragma once

namespace Hierarchy
{
	class Node
	{
	public:
		Node();
		~Node() = default;

		virtual void Render();

		void OnNodeSelected(Node* node);

		void SetSelect(bool select);
		void SetParent(Node* node);

		bool IsSelected() const;
		std::shared_ptr<Resource::Property> GetProperty() const;
		Node* GetParent() const;

	private:
		void RenderContextMenu();

	protected:
		static constexpr auto DEFAULT_NODE_NAME{ "NewNode" };

		bool m_isSelected;
		std::shared_ptr<Resource::Property> m_property;

		Node* m_parent;
		std::vector<std::unique_ptr<Node>> m_children;
	};

	class RootNode : public Node
	{
	public:
		RootNode();
		~RootNode() = default;

		virtual void Render() final;

		void OnNodeSelected(Node* node);

	private:
		void RenderContextMenu();

	private:
		std::filesystem::path m_filePath; // 파일이 저장될 절대 경로
		bool m_isModified; // 저장된 이후 변경점이 있는지
	};

	class Window : public TSingleton<Window>
	{
	public:
		void Render();

		// 노드가 선택될 때 선택된 노드를 전달받는다.
		void OnNodeSelected(Node* const node);

	private:
		void ProcessDragDrop();
		void RenderMenu();
		void RenderNode();

		// 프로젝트 윈도우로부터 드래드 드랍으로 파일 절대 경로를 전달받는다.
		void OnFileDragDrop(std::string_view path);

		// 각 메뉴 선택 시 호출된다.
		void OnMenuFileNew();
		void OnMenuFileOpen();
		void OnMenuFileSave();
		void OnMenuFileSaveAs();

	private:
		static constexpr auto WINDOW_NAME{ "Hierarchy" };
		static constexpr auto MENU_FILE{ "File" };
		static constexpr auto MENU_FILE_NEW{ "New" };
		static constexpr auto MENU_FILE_OPEN{ "Open" };
		static constexpr auto MENU_FILE_SAVE{ "Save" };
		static constexpr auto MENU_FILE_SAVEAS{ "Save as" };
		std::vector<std::unique_ptr<RootNode>> m_roots;
	};
}