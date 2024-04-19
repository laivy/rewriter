#include "Stdafx.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "PropInfo.h"
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
	m_prop{ nullptr }
{
	m_onPropertySelect = { std::bind_front(&Inspector::OnPropertySelect, this) };
	Global::OnPropertySelect.Add(&m_onPropertySelect);

	m_onPropertyDelete = { std::bind_front(&Inspector::OnPropertyDelete, this) };
	Global::OnPropertyDelete.Add(&m_onPropertyDelete);
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

void Inspector::OnPropertyDelete(std::shared_ptr<Resource::Property> prop)
{
	if (m_prop == prop)
		m_prop.reset();
}

void Inspector::OnPropertySelect(std::shared_ptr<Resource::Property> prop)
{
	m_prop = prop;
}

void Inspector::RenderBasicInfo()
{
	if (!m_prop)
		return;

	ImGui::AlignTextToFramePadding();
	ImGui::SeparatorText("Property Info");
	if (Global::propInfo[m_prop].isRoot)
	{
		ImGui::Text("Path");
		ImGui::SameLine(100);
		ImGui::Selectable(Global::propInfo[m_prop].path.string().empty() ? "-" : Global::propInfo[m_prop].path.string().c_str(), true);
	}
	else
	{
		ImGui::Text("Name");
		ImGui::SameLine(100);
		auto name{ Util::wstou8s(m_prop->GetName()) };
		if (ImGui::InputText("##NAME", &name, ImGuiInputTextFlags_EnterReturnsTrue))
			m_prop->SetName(Util::u8stows(name));

		ImGui::Text("Type"); ImGui::SameLine(100);
		if (ImGui::BeginCombo("##INSPECTOR/TYPE", m_prop ? TypeToString(m_prop->GetType()).c_str() : "-"))
		{
			for (const auto& s : PROPERTY_TYPES)
			{
				if (ImGui::Selectable(s))
				{
					auto type{ StringToType(s) };
					m_prop->SetType(type);
				}
			}
			ImGui::EndCombo();
		}
	}

	auto type{ m_prop->GetType() };
	if (type == Resource::Property::Type::FOLDER)
		return;

	ImGui::Text("Value"); ImGui::SameLine(100);
	switch (type)
	{
	case Resource::Property::Type::INT:
	{
		auto data{ m_prop->GetInt() };
		if (ImGui::InputInt("##INSPECTOR/INT", &data))
			m_prop->Set(data);
		break;
	}
	case Resource::Property::Type::INT2:
	{
		auto data{ m_prop->GetInt2() };
		if (ImGui::InputInt2("##INSPECTOR/INT2", reinterpret_cast<int*>(&data)))
			m_prop->Set(data);
		break;
	}
	case Resource::Property::Type::FLOAT:
	{
		auto data{ m_prop->GetFloat() };
		if (ImGui::InputFloat("##INSPECTOR/FLOAT", &data))
			m_prop->Set(data);
		break;
	}
	case Resource::Property::Type::STRING:
	{
		auto data{ Util::wstou8s(m_prop->GetString()) };
		if (ImGui::InputTextMultiline("##INSPECTOR/STRING", &data))
			m_prop->Set(Util::u8stows(data));
		break;
	}
	}
}
