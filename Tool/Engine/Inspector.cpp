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
	m_float{}
{
	m_name.resize(STRING_LENGTH_MAX);
	m_string.resize(STRING_LENGTH_MAX);
	//Event::OnNodeDelete.Add(std::bind_front(&Inspector::OnNodeDelete, this));
	//Event::OnNodeSelect.Add(std::bind_front(&Inspector::OnNodeSelect, this));
}

void Inspector::Update(float deltaTime)
{
}

void Inspector::Render()
{
	if (ImGui::Begin(WINDOW_NAME))
	{
		ImGui::PushID("Inspector");
		RenderBasicInfo();
		ImGui::PopID();
	}
	ImGui::End();
}

Node* Inspector::GetNode() const
{
	return m_node;
}

bool Inspector::OnNodeDelete(Node* node)
{
	if (m_node == node)
		m_node = nullptr;
	return false;
}

bool Inspector::OnNodeSelect(Node* node)
{
	m_node = node;
	if (!m_node)
		return false;

	// 이름 설정
	if (node->IsRoot())
	{
		m_name = node->GetFilePath().filename().string();
	}
	else
	{
		m_name = node->GetName();
	}
	m_name.resize(STRING_LENGTH_MAX);

	// 값 설정
	switch (m_node->GetType())
	{
	case Resource::Property::Type::INT:
		m_int = m_node->GetInt();
		break;
	case Resource::Property::Type::INT2:
	{
		m_int2 = m_node->GetInt2();
		break;
	}
	case Resource::Property::Type::FLOAT:
		m_float = m_node->GetFloat();
		break;
	case Resource::Property::Type::STRING:
		m_string = m_node->GetString();
		break;
	}

	return false;
}

void Inspector::RenderBasicInfo()
{
	if (!m_node)
		return;

	ImGui::SeparatorText("Property Info");

	ImGuiInputTextFlags flag{ ImGuiInputTextFlags_CharsNoBlank };
	if (m_node->IsRoot())
		flag |= ImGuiInputTextFlags_ReadOnly;
	ImGui::AlignTextToFramePadding();

	ImGui::Text("Name"); ImGui::SameLine(100);
	if (ImGui::InputText("##Name", m_name.data(), STRING_LENGTH_MAX + 1, flag))
	{
		m_node->SetName(m_name);
	}

	if (m_node->IsRoot())
	{
		ImGui::Text("FilePath"); ImGui::SameLine(100);

		auto path{ m_node->GetFilePath() };
		if (path.is_absolute())
			ImGui::Text(path.string().c_str());
		else
			ImGui::Text("-");
		return;
	}

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Type"); ImGui::SameLine(100);
	if (ImGui::BeginCombo("##Inspector/Type", m_node ? TypeToString(m_node->GetType()).c_str() : "-"))
	{
		for (const std::string& s : PROPERTY_TYPES)
		{
			if (ImGui::Selectable(s.c_str()))
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
		if (ImGui::InputInt("##Inspector/Int", &m_int))
			m_node->Set(m_int);
		break;
	}
	case Resource::Property::Type::INT2:
	{
		if (ImGui::InputInt2("##Inspector/Int2", reinterpret_cast<int*>(&m_int2)))
			m_node->Set(m_int2);
		break;
	}
	case Resource::Property::Type::FLOAT:
	{
		if (ImGui::InputFloat("##Inspector/Float", &m_float))
			m_node->Set(m_float);
		break;
	}
	case Resource::Property::Type::STRING:
	{
		if (ImGui::InputText("##Inspector/String", m_string.data(), STRING_LENGTH_MAX + 1))
		{
			std::wstring wstring{};
			wstring.assign(m_string.begin(), m_string.end());
			m_node->Set(wstring);
		}
		break;
	}
	}
}
