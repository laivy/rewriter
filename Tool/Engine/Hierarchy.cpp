#include "Stdafx.h"
#include "Hierarchy.h"
#include "Inspector.h"

namespace Hierarchy
{
	Node::Node() :
		m_parent{ nullptr },
		m_type{ Resource::Property::Type::GROUP },
		m_isSelected{ false }
	{
	}

	void Node::Render()
	{
		// 자식 노드가 없으면 Selectable
		if (m_children.empty())
		{
			if (ImGui::Selectable(m_name.c_str(), &m_isSelected))
			{
				if (auto w{ Window::GetInstance() })
					w->OnNodeSelected(this);
			}
			RenderContextMenu();
			return;
		}

		// 자식 노드가 있으면 TreeNode
		ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnDoubleClick };
		if (IsSelected())
			flag |= ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx(m_name.c_str(), flag))
		{
			if (ImGui::IsItemClicked())
			{
				if (auto w{ Window::GetInstance() })
					w->OnNodeSelected(this);
			}

			RenderContextMenu();

			for (const auto& p : m_children)
				p->Render();

			ImGui::TreePop();
		}
	}

	void Node::OnNodeSelected(Node* node)
	{
		// Ctrl키를 누르고 있어야 다중 선택 가능
		if (ImGui::GetIO().KeyCtrl)
		{
			// 트리 노드의 경우는 선택됨을 수동으로 설정해야함
			if (!m_children.empty() && this == node)
				SetSelect(true);
			return;
		}

		// 선택된 노드를 제외하고 모든 노드 선택 상태 해제
		SetSelect(this == node ? true : false);
		for (auto& child : m_children)
			child->OnNodeSelected(node);
	}

	void Node::SetParent(Node* node)
	{
		m_parent = node;
	}

	void Node::SetName(std::string_view name)
	{
		m_name = name;
	}

	void Node::SetType(Resource::Property::Type type)
	{
		m_type = type;
	}

	void Node::SetSelect(bool select)
	{
		m_isSelected = select;
	}

	Node* Node::GetParent() const
	{
		return m_parent;
	}

	std::string Node::GetName() const
	{
		return m_name;
	}

	Resource::Property::Type Node::GetType() const
	{
		return m_type;
	}

	bool Node::IsSelected() const
	{
		return m_isSelected;
	}

	void Node::RenderContextMenu()
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Selectable("Add"))
			{
				int number{ 1 };
				std::string name{ DEFAULT_NODE_NAME };
				while (true)
				{
					auto it{ std::ranges::find_if(m_children, [&name](const auto& p) { return p->GetName() == name; }) };
					if (it == m_children.cend())
						break;
					name = std::format("{}{}", DEFAULT_NODE_NAME, number++);
				}

				auto node{ std::make_unique<Node>() };
				node->SetParent(this);
				node->SetName(name);
				m_children.push_back(std::move(node));
			}
			ImGui::EndPopup();
		}
	}

	RootNode::RootNode() : m_isModified{ false }
    {
    }

	void RootNode::Render()
	{
		if (ImGui::CollapsingHeader(m_name.c_str(), ImGuiTreeNodeFlags_OpenOnDoubleClick))
		{
			RenderContextMenu();

			if (ImGui::IsItemClicked())
			{
				if (auto w{ Window::GetInstance() })
					w->OnNodeSelected(this);
			}

			ImGui::Indent();
			for (const auto& node : m_children)
				node->Render();
			ImGui::Unindent();
		}
	}

	void RootNode::OnNodeSelected(Node* node)
	{
		for (auto& child : m_children)
			child->OnNodeSelected(node);
	}

	void RootNode::RenderContextMenu()
	{
		if (ImGui::BeginPopupContextItem(m_name.c_str()))
		{
			if (ImGui::Selectable("Add"))
			{
				int index{ 1 };
				std::string name{ DEFAULT_NODE_NAME };
				while (true)
				{
					auto it{ std::ranges::find_if(m_children, [&name](const auto& p) { return p->GetName() == name; }) };
					if (it == m_children.cend())
						break;
					name = std::format("{}{}", DEFAULT_NODE_NAME, index++);
				}

				auto node{ std::make_unique<Node>() };
				node->SetParent(this);
				node->SetName(name);
				m_children.push_back(std::move(node));
			}
			ImGui::EndPopup();
		}
	}

	void Window::Render()
	{
		if (ImGui::Begin(WINDOW_NAME, NULL, ImGuiWindowFlags_MenuBar))
		{
			ProcessDragDrop();
			RenderMenu();
			RenderNode();
		}
		ImGui::End();
	}

	void Window::RenderMenu()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(MENU_FILE))
			{
				if (ImGui::MenuItem(MENU_FILE_NEW))
					OnMenuFileNew();
				if (ImGui::MenuItem(MENU_FILE_OPEN))
					OnMenuFileOpen();
				if (ImGui::MenuItem(MENU_FILE_SAVE))
					OnMenuFileSave();
				if (ImGui::MenuItem(MENU_FILE_SAVEAS))
					OnMenuFileSaveAs();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void Window::RenderNode()
	{
		for (const auto& root : m_roots)
			root->Render();
	}

	void Window::ProcessDragDrop()
	{
		auto window{ ImGui::GetCurrentWindow() };
		if (ImGui::BeginDragDropTargetCustom(window->ContentRegionRect, window->ID))
		{
			if (auto payload{ ImGui::AcceptDragDropPayload("FILE_TO_HIERARCHY") })
			{
				std::string_view filePath{ static_cast<const char*>(payload->Data) };
				OnFileDragDrop(filePath.data());
			}
			ImGui::EndDragDropTarget();
		}
	}

	void Window::OnFileDragDrop(std::string_view path)
	{
	}

	void Window::OnMenuFileNew()
	{
		int index{ 1 };
		std::string name{ "NewFile" };
		while (true)
		{
			auto it{ std::ranges::find_if(m_roots, [&name](const auto& p) { return p->GetName() == name; }) };
			if (it == m_roots.cend())
				break;
			name = std::format("NewFile{}", index++);
		}

		auto prop{ std::make_unique<RootNode>() };
		prop->SetName(name);
		m_roots.push_back(std::move(prop));
	}

	void Window::OnMenuFileOpen()
	{
	}

	void Window::OnMenuFileSave()
	{
	}

	void Window::OnMenuFileSaveAs()
	{
	}

	void Window::OnNodeSelected(Node* const node)
	{
		for (const auto& root : m_roots)
			root->OnNodeSelected(node);

		// 인스펙터 윈도우에 알려줌
		if (auto w{ Inspector::Window::GetInstance() })
			w->OnNodeSelected(node);
	}
}