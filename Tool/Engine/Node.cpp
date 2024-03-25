#include "Stdafx.h"
#include "App.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Node.h"

Node::Node() :
	m_id{ s_index++ },
	m_isSelected{ false },
	m_isValid{ true },
	m_parent{ nullptr },
	m_property{ nullptr }
{
}

std::vector<std::unique_ptr<Node>>::iterator Node::begin()
{
	return m_children.begin();
}

std::vector<std::unique_ptr<Node>>::iterator Node::end()
{
	return m_children.end();
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
			; //OnNodeSelect.Notify(this);
		
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
			; //Event::OnNodeSelect.Notify(this);

		RenderContextMenu();

		for (const auto& p : m_children)
			p->Render();
		ImGui::TreePop();
	}
}

void Node::Add(std::unique_ptr<Node> child)
{
	//Event::OnNodeAdd.Notify(child.get());
	m_property->Add(child->m_property);
	m_children.push_back(std::move(child));
}

void Node::Delete()
{
	//Event::OnNodeDelete.Notify(this);
	m_isValid = false;
}

void Node::DeleteInvalidChildren()
{
	std::erase_if(m_children, [](const auto& c) { return !c->IsValid(); });
	for (const auto& child : m_children)
		child->DeleteInvalidChildren();
}

void Node::SetProperty(const std::shared_ptr<Resource::Property>& prop)
{
	m_property = prop;
	if (!m_property)
		return;
	for (const auto& [_, child] : *m_property)
	{
		auto node{ std::make_unique<Node>() };
		node->SetProperty(child);
		node->SetParent(this);
		m_children.push_back(std::move(node));
	}
}

void Node::SetType(Resource::Property::Type type)
{
	m_property->SetType(type);
	switch (type)
	{
	case Resource::Property::Type::INT:
		m_property->Set(0);
		break;
	case Resource::Property::Type::INT2:
		m_property->Set(INT2{});
		break;
	case Resource::Property::Type::FLOAT:
		m_property->Set(0.0f);
		break;
	case Resource::Property::Type::STRING:
		m_property->Set(L"");
		break;
	}
}

void Node::SetName(const std::string& name)
{
	std::wstring temp{};
	temp.assign(name.begin(), name.end());
	m_property->SetName(temp);
}

void Node::SetParent(Node* parent)
{
	m_parent = parent;
}

void Node::SetFilePath(const std::filesystem::path& path)
{
	m_filePath = path;
}

void Node::SetSelect(bool isSelect)
{
	m_isSelected = isSelect;
}

bool Node::IsRoot() const
{
	return m_parent ? false : true;
}

Resource::Property::Type Node::GetType() const
{
	if (m_property)
		return m_property->GetType();
	return Resource::Property::Type::FOLDER;
}

std::string Node::GetName() const
{
	if (m_property)
	{
		std::wstring name{ m_property->GetName() };
		std::string temp(name.size() * sizeof(wchar_t) + 1, '\0');
		size_t length{ 0 };
		::wcstombs_s(&length, temp.data(), temp.size(), name.data(), name.size());
		return temp.substr(0, length - 1);
	}
	return "";
}

int Node::GetInt() const
{
	if (m_property)
		return m_property->GetInt();
	return 0;
}

INT2 Node::GetInt2() const
{
	if (m_property)
		return m_property->GetInt2();
	return INT2{};
}

float Node::GetFloat() const
{
	if (m_property)
		return m_property->GetFloat();
	return 0.0f;
}

std::string Node::GetString() const
{
	if (m_property)
	{
		std::wstring str{ m_property->GetString() };
		std::string temp(str.size() * sizeof(wchar_t) + 1, '\0');
		size_t length{ 0 };
		::wcstombs_s(&length, temp.data(), temp.size(), str.data(), str.size());
		return temp.substr(0, length - 1);
	}
	return "";
}

std::shared_ptr<Resource::Image> Node::GetImage() const
{
	if (m_property)
		return m_property->GetImage();
	return nullptr;
}

Node* Node::GetParent() const
{
	return m_parent;
}

std::filesystem::path Node::GetFilePath() const
{
	return m_filePath;
}

bool Node::IsSelected() const
{
	return m_isSelected;
}

bool Node::IsValid() const
{
	return m_isValid;
}

void Node::RenderContextMenu()
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
			node->SetProperty(std::make_shared<Resource::Property>());
			node->SetName(name);
			node->SetParent(this);
			Add(std::move(node));
		}
		if (ImGui::Selectable("Del"))
		{
			Delete();
		}
		ImGui::EndPopup();
	}
}

//void RootNode::Render()
//{
//	if (ImGui::CollapsingHeader(std::format("{}##{}", m_filePath.filename().string(), m_id).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick))
//	{
//		if (ImGui::IsItemClicked())
//			; //Event::OnNodeSelect.Notify(this);
//
//		RenderContextMenu();
//
//		ImGui::Indent();
//		for (const auto& node : m_children)
//			node->Render();
//		ImGui::Unindent();
//	}
//}
//
//bool RootNode::IsRootNode() const
//{
//	return true;
//}
//
//void RootNode::Save() const
//{
//	std::string name{ GetName() };
//	std::wstring temp{};
//	temp.assign(name.begin(), name.end());
//
//	std::array<wchar_t, MAX_PATH> filename{};
//	std::copy(temp.begin(), temp.end(), filename.begin());
//
//	OPENFILENAME ofn{};
//	ofn.lStructSize = sizeof(ofn);
//	ofn.lpstrFilter = L"Data Files (*.dat)\0*.dat\0";
//	ofn.lpstrFile = filename.data();
//	ofn.nMaxFile = MAX_PATH;
//	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
//	ofn.lpstrDefExt = L"dat";
//	GetSaveFileName(&ofn);
//
//	m_property->Save(ofn.lpstrFile);
//}
//
//void RootNode::SetFilePath(const std::filesystem::path& filePath)
//{
//	m_filePath = filePath;
//}
//
//std::filesystem::path RootNode::GetFilePath() const
//{
//	return m_filePath;
//}
//
//void RootNode::RenderContextMenu()
//{
//	if (ImGui::BeginPopupContextItem(std::format("Property#{}", m_id).c_str()))
//	{
//		if (ImGui::Selectable("Add"))
//		{
//			int index{ 1 };
//			std::string name{ DEFAULT_NODE_NAME };
//			while (true)
//			{
//				auto it{ std::ranges::find_if(m_children,
//					[&name](const auto& p)
//					{
//						return p->GetName() == name;
//					}) };
//				if (it == m_children.cend())
//					break;
//				name = std::format("{}{}", DEFAULT_NODE_NAME, index++);
//			}
//
//			auto node{ std::make_unique<Node>() };
//			node->SetProperty(std::make_shared<Resource::Property>());
//			node->SetName(name);
//			node->SetParent(this);
//			Add(std::move(node));
//		}
//		ImGui::EndPopup();
//	}
//}
