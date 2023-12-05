#include "Stdafx.h"
#include "Hierarchy.h"
#include "Inspector.h"

Resource::Property::Type StringToType(std::string_view type)
{
	// GROUP, INT, INT2, FLOAT, STRING, IMAGE
	if (type == "GROUP")
		return Resource::Property::Type::GROUP;
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

	return Resource::Property::Type::GROUP;
}

std::string TypeToString(Resource::Property::Type type)
{
	switch (type)
	{
	case Resource::Property::Type::GROUP:
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

namespace Inspector
{
	Window::Window() :
		m_node{ nullptr },
		m_name{ "-" },
		m_type{ Resource::Property::Type::GROUP }
	{
	}

	void Window::Render()
	{
		if (ImGui::Begin(WINDOW_NAME))
		{
			ImGui::SeparatorText("Basic Info");

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Name"); ImGui::SameLine(100);
			ImGuiInputTextFlags flags{ ImGuiInputTextFlags_EnterReturnsTrue };
			if (!m_node)
				flags |= ImGuiInputTextFlags_ReadOnly;
			if (ImGui::InputText("##Name", m_name.data(), m_name.size(), flags))
			{
				if (m_node)
					Resource::SetName(m_node->GetProperty(), m_name.data());
			}

			ImGui::Text("Type"); ImGui::SameLine(100);
			if (ImGui::BeginCombo("##Type", m_node ? TypeToString(Resource::GetType(m_node->GetProperty())).c_str() : "-"))
			{
				for (const std::string& s : { "GROUP", "INT", "INT2", "FLOAT", "STRING", "IMAGE" })
				{
					if (ImGui::Selectable(s.c_str()) && m_node)
						Resource::SetType(m_node->GetProperty(), StringToType(s));
				}
				ImGui::EndCombo();
			}
		}
		ImGui::End();
	}

	void Window::OnNodeSelected(Hierarchy::Node* node)
	{
		m_node = node;
		if (!m_node)
			return;

		std::string name{ Resource::GetName(m_node->GetProperty()) };
		name.resize(BUFFER_SIZE);
		std::ranges::copy(name, m_name.data());
	}
}