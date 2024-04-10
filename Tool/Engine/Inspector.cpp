#include "Stdafx.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "ProfInfo.h"
#include "Util.h"

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
	m_prop{ nullptr },
	m_int{},
	m_int2{},
	m_float{}
{
	m_name.resize(STRING_LENGTH_MAX);
	m_string.resize(STRING_LENGTH_MAX);

	m_onNodeSelect = { std::bind_front(&Inspector::OnNodeSelect, this) };
	Global::OnPropertySelect.Add(&m_onNodeSelect);

	m_onNodeDelete = { std::bind_front(&Inspector::OnNodeDelete, this) };
	Global::OnPropertyDelete.Add(&m_onNodeDelete);
}

void Inspector::Update(float deltaTime)
{
}

void Inspector::Render()
{
	ImGui::PushID(WINDOW_NAME);
	if (ImGui::Begin(WINDOW_NAME))
	{
		RenderBasicInfo();
	}
	ImGui::End();
	ImGui::PopID();
}

std::shared_ptr<Resource::Property> Inspector::GetNode() const
{
	return m_prop;
}

void Inspector::OnNodeDelete(std::shared_ptr<Resource::Property> prop)
{
	if (m_prop == prop)
		m_prop.reset();
}

void Inspector::OnNodeSelect(std::shared_ptr<Resource::Property> prop)
{
	m_prop = prop;
	if (!m_prop)
		return;

	// 이름 설정
	m_name = Util::wstou8s(prop->name);

	// 값 설정
	switch (m_prop->GetType())
	{
	case Resource::Property::Type::INT:
	{
		m_int = m_prop->GetInt();
		break;
	}
	case Resource::Property::Type::INT2:
	{
		m_int2 = m_prop->GetInt2();
		break;
	}
	case Resource::Property::Type::FLOAT:
	{
		m_float = m_prop->GetFloat();
		break;
	}
	case Resource::Property::Type::STRING:
	{
		m_string = Util::wstou8s(m_prop->GetString());
		break;
	}
	}
}

void Inspector::RenderBasicInfo()
{
	if (!m_prop)
		return;

	ImGui::SeparatorText("Property Info");

	ImGuiInputTextFlags flag{ ImGuiInputTextFlags_CharsNoBlank };
	//if (!m_prop->path.empty())
	//	flag |= ImGuiInputTextFlags_ReadOnly;
	ImGui::AlignTextToFramePadding();

	ImGui::Text("Name"); ImGui::SameLine(100);
	if (ImGui::InputText("##Name", m_name.data(), STRING_LENGTH_MAX + 1, flag))
	{
		m_prop->SetName(Util::u8stows(m_name));
	}

	//if (!m_prop->path.empty())
	//{
	//	ImGui::Text("FilePath"); ImGui::SameLine(100);
	//	if (m_prop->path.is_absolute())
	//		ImGui::Text(m_prop->path.string().c_str());
	//	else
	//		ImGui::Text("-");
	//	ImGui::PopID();
	//	return;
	//}

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Type"); ImGui::SameLine(100);
	if (ImGui::BeginCombo("##Inspector/Type", m_prop ? TypeToString(m_prop->GetType()).c_str() : "-"))
	{
		for (const std::string& s : PROPERTY_TYPES)
		{
			if (ImGui::Selectable(s.c_str()))
				m_prop->SetType(StringToType(s));
		}
		ImGui::EndCombo();
	}

	auto type{ m_prop->GetType() };
	if (type == Resource::Property::Type::FOLDER)
		return;

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Value"); ImGui::SameLine(100);

	switch (type)
	{
	case Resource::Property::Type::INT:
	{
		if (ImGui::InputInt("##Inspector/Int", &m_int))
			m_prop->Set(m_int);
		break;
	}
	case Resource::Property::Type::INT2:
	{
		if (ImGui::InputInt2("##Inspector/Int2", reinterpret_cast<int*>(&m_int2)))
			m_prop->Set(m_int2);
		break;
	}
	case Resource::Property::Type::FLOAT:
	{
		if (ImGui::InputFloat("##Inspector/Float", &m_float))
			m_prop->Set(m_float);
		break;
	}
	case Resource::Property::Type::STRING:
	{
		if (ImGui::InputText("##Inspector/String", m_string.data(), STRING_LENGTH_MAX + 1))
			m_prop->Set(Util::u8stows(m_string));
		break;
	}
	}
}
