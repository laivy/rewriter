#include "Stdafx.h"
#include "App.h"
#include "Node.h"
#include "Hierarchy.h"
#include "Inspector.h"

Node::Node() :
	m_id{ s_index++ },
	m_parent{ nullptr },
	m_isSelected{ false }
{
	m_property = Resource::Create();
}

void Node::Render()
{
	// 자식 노드가 없으면 Selectable
	if (m_children.empty())
	{
		auto name{ GetName() };
		if (name.starts_with('\0'))
			name = "##";
		if (ImGui::Selectable(std::format("{}##{}", name, m_id).c_str(), &m_isSelected))
			OnNodeSelected();
		
		RenderContextMenu();
		return;
	}

	// 자식 노드가 있으면 TreeNode
	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick };
	if (m_isSelected)
		flag |= ImGuiTreeNodeFlags_Selected;
	if (ImGui::TreeNodeEx(std::format("{}##{}", GetName(), m_id).c_str(), flag))
	{
		if (ImGui::IsItemClicked())
			OnNodeSelected();

		RenderContextMenu();

		for (const auto& p : m_children)
			p->Render();
		ImGui::TreePop();
	}
}

bool Node::IsRootNode() const
{
	return false;
}

std::vector<std::unique_ptr<Node>>::iterator Node::begin()
{
	return m_children.begin();
}

std::vector<std::unique_ptr<Node>>::iterator Node::end()
{
	return m_children.end();
}

void Node::OnNodeSelected()
{
	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->OnNodeSelected(this);
	if (auto inspector{ Inspector::GetInstance() })
		inspector->OnNodeSelected(this);
}

void Node::SetParent(Node* parent)
{
	m_parent = parent;
}

void Node::SetType(Resource::Property::Type type)
{
	Resource::SetType(m_property, type);
}

void Node::SetName(const std::string& name)
{
	Resource::SetName(m_property, name);
}

void Node::SetSelect(bool isSelect)
{
	m_isSelected = isSelect;
}

Resource::Property::Type Node::GetType() const
{
	if (m_property)
		return Resource::GetType(m_property);
	return Resource::Property::Type::FOLDER;
}

std::string Node::GetName() const
{
	return Resource::GetName(m_property);
}

int Node::GetInt() const
{
	return Resource::GetInt(m_property);
}

INT2 Node::GetInt2() const
{
	return Resource::GetInt2(m_property);
}

float Node::GetFloat() const
{
	return Resource::GetFloat(m_property);
}

std::string Node::GetString() const
{
	return Resource::GetString(m_property);
}

std::shared_ptr<Resource::Image> Node::GetImage() const
{
	return Resource::GetImage(m_property);
}

Node* Node::GetParent() const
{
	return m_parent;
}

bool Node::IsSelected() const
{
	return m_isSelected;
}

void Node::RenderContextMenu()
{
	if (ImGui::BeginPopupContextItem(std::format("Property#{}", m_id).c_str()))
	{
		if (ImGui::Selectable("Add"))
		{
			int number{ 1 };
			std::string name{ DEFAULT_NODE_NAME };
			while (true)
			{
				auto it{ std::ranges::find_if(m_children,
					[&name](const auto& p)
					{
						return p->GetName() == name;
					}) };
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

void RootNode::Render()
{
	if (ImGui::CollapsingHeader(std::format("{}##{}", GetName(), m_id).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick))
	{
		if (ImGui::IsItemClicked())
			OnNodeSelected();

		RenderContextMenu();

		ImGui::Indent();
		for (const auto& node : m_children)
			node->Render();
		ImGui::Unindent();
	}
}

bool RootNode::IsRootNode() const
{
	return true;
}

void RootNode::Save() const
{
	std::string name{ GetName() };
	std::wstring temp{};
	temp.assign(name.begin(), name.end());

	std::array<wchar_t, MAX_PATH> filename{};
	std::copy(temp.begin(), temp.end(), filename.begin());

	OPENFILENAME ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
	ofn.lpstrFile = filename.data();
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = (LPCWSTR)L"dat";
	GetSaveFileName(&ofn);

	std::wstring wstr{ ofn.lpstrFile };
	std::string str{};
	str.assign(wstr.begin(), wstr.end());
	//Resource::Save(m_property, str);
}

void RootNode::RenderContextMenu()
{
	if (ImGui::BeginPopupContextItem(std::format("Property#{}", m_id).c_str()))
	{
		if (ImGui::Selectable("Add"))
		{
			int index{ 1 };
			std::string name{ DEFAULT_NODE_NAME };
			while (true)
			{
				auto it{ std::ranges::find_if(m_children,
					[&name](const auto& p)
					{
						return p->GetName() == name;
					}) };
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
