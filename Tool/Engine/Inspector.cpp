#include "Stdafx.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Node.h"

Resource::Property::Type StringToType(std::string_view type)
{
	if (type == "GROUP")
		return Resource::Property::Type::FOLDER;
	if (type == "INT")
		return Resource::Property::Type::INT;
	if (type == "INT2")
		return Resource::Property::Type::INT2;
	if (type == "FLOAT")
		return Resource::Property::Type::FLOAT;
	if (type == "STRING")
		return Resource::Property::Type::STRING;
	if (type == "IMAGE")
		return Resource::Property::Type::IMAGE;
	return Resource::Property::Type::FOLDER;
}

std::string TypeToString(Resource::Property::Type type)
{
	switch (type)
	{
	case Resource::Property::Type::FOLDER:
		return "GROUP";
	case Resource::Property::Type::INT:
		return "INT";
	case Resource::Property::Type::INT2:
		return "INT2";
	case Resource::Property::Type::FLOAT:
		return "FLOAT";
	case Resource::Property::Type::STRING:
		return "STRING";
	case Resource::Property::Type::IMAGE:
		return "IMAGE";
	default:
		return "";
	}
}

Inspector::Inspector() :
	m_node{ nullptr },
	m_int{},
	m_int2{},
	m_float{},
	m_string{}
{
	m_name.resize(STRING_LENGTH_MAX);
	m_string.resize(STRING_LENGTH_MAX);
}

void Inspector::Render()
{
	if (ImGui::Begin(WINDOW_NAME))
	{
		RenderBasicInfo();
	}
	ImGui::End();
}

void Inspector::OnNodeSelected(Node* node)
{
	m_node = node;
	if (!m_node)
		return;

	// 이름 설정
	m_name = node->GetName();
	m_name.resize(STRING_LENGTH_MAX);

	// 값 설정
	switch (m_node->GetType())
	{
	case Resource::Property::Type::INT:
		m_int = m_node->GetInt();
		break;
	case Resource::Property::Type::INT2:
	{
		auto int2{ m_node->GetInt2() };
		m_int2[0] = int2.x;
		m_int2[1] = int2.y;
		break;
	}
	case Resource::Property::Type::FLOAT:
		m_float = m_node->GetFloat();
		break;
	case Resource::Property::Type::STRING:
		m_string = m_node->GetString();
		break;
	}
}

Node* Inspector::GetNode() const
{
	return m_node;
}

void Inspector::RenderBasicInfo()
{
	if (!m_node)
		return;

	ImGui::SeparatorText("Property Info");

	ImGuiInputTextFlags flag{ ImGuiInputTextFlags_CharsNoBlank };
	if (m_node->IsRootNode())
		flag |= ImGuiInputTextFlags_ReadOnly;
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Name"); ImGui::SameLine(100);
	if (ImGui::InputText("##Name", m_name.data(), STRING_LENGTH_MAX + 1, flag))
	{
		m_node->SetName(m_name);
	}

	if (m_node->IsRootNode())
		return;

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Type"); ImGui::SameLine(100);
	if (ImGui::BeginCombo("##Type", m_node ? TypeToString(m_node->GetType()).c_str() : "-"))
	{
		for (const std::string& s : PROPERTY_TYPES)
		{
			if (ImGui::Selectable(s.c_str()) && m_node)
				m_node->SetType(StringToType(s));
		}
		ImGui::EndCombo();
	}

	auto type{ m_node->GetType() };
	if (type == Resource::Property::Type::FOLDER)
		return;

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Value"); ImGui::SameLine(100);

	switch (type)
	{
	case Resource::Property::Type::INT:
	{
		if (ImGui::InputInt("##Int", &m_int))
			m_node->Set(m_int);
		break;
	}
	case Resource::Property::Type::INT2:
	{
		if (ImGui::InputInt2("##Int2", m_int2.data()))
			m_node->Set(INT2{ m_int2[0], m_int2[1] });
		break;
	}
	case Resource::Property::Type::FLOAT:
	{
		if (ImGui::InputFloat("##Float", &m_float))
			m_node->Set(m_float);
		break;
	}
	case Resource::Property::Type::STRING:
	{
		if (ImGui::InputText("##String", m_string.data(), STRING_LENGTH_MAX + 1))
			m_node->Set(m_string);
		break;
	}
	}
}
