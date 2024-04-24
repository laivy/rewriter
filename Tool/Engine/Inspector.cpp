#include "Stdafx.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "PropInfo.h"
#include "Util.h"

std::map<Resource::Property::Type, std::string> PROPERTY_TYPES
{
	{ Resource::Property::Type::FOLDER, "GROUP" },
	{ Resource::Property::Type::INT, "INT" },
	{ Resource::Property::Type::INT2, "INT2" },
	{ Resource::Property::Type::FLOAT, "FLOAT" },
	{ Resource::Property::Type::STRING, "STRING" },
	{ Resource::Property::Type::IMAGE, "IMAGE" }
};

Inspector::Inspector()
{
	m_onPropertySelect = Global::OnPropertySelect.Add(std::bind_front(&Inspector::OnPropertySelect, this));
	m_onPropertyDelete = Global::OnPropertyDelete.Add(std::bind_front(&Inspector::OnPropertyDelete, this));
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

void Inspector::OnPropertyDelete(std::shared_ptr<Resource::Property> prop)
{
	if (m_prop.lock() == prop)
		m_prop.reset();
}

void Inspector::OnPropertySelect(std::shared_ptr<Resource::Property> prop)
{
	m_prop = prop;
}

void Inspector::RenderBasicInfo()
{
	auto prop{ m_prop.lock() };
	if (!prop)
		return;

	ImGui::AlignTextToFramePadding();
	ImGui::SeparatorText("Property Info");
	if (Global::propInfo[prop].isRoot)
	{
		ImGui::Text("Path");
		ImGui::SameLine(100);
		if (Global::propInfo[prop].path.empty())
			ImGui::InputText("##PATH", "-", 1, ImGuiInputTextFlags_ReadOnly);
		else
			ImGui::InputText("##PATH", &Util::u8stou8s(Global::propInfo[prop].path.u8string()), ImGuiInputTextFlags_ReadOnly);
	}
	else
	{
		ImGui::Text("Name");
		ImGui::SameLine(100);
		auto name{ Util::wstou8s(prop->GetName()) };
		if (ImGui::InputText("##NAME", &name, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			do
			{
				if (auto parent{ Global::propInfo[prop].parent.lock() })
				{
					auto it{ std::ranges::find_if(parent->children, [&name](const auto& prop) { return prop->name == Util::u8stows(name); }) };
					if (it != parent->children.cend())
						break;
				}
				prop->SetName(Util::u8stows(name));
			} while (false);
		}

		ImGui::Text("Type");
		ImGui::SameLine(100);
		if (ImGui::BeginCombo("##INSPECTOR/TYPE", prop ? PROPERTY_TYPES.at(prop->GetType()).c_str() : "-"))
		{
			for (const auto& [e, s] : PROPERTY_TYPES)
			{
				if (ImGui::Selectable(s.c_str()))
					prop->SetType(e);
			}
			ImGui::EndCombo();
		}
	}

	auto type{ prop->GetType() };
	if (type == Resource::Property::Type::FOLDER)
		return;

	ImGui::Text("Value");
	ImGui::SameLine(100);
	switch (type)
	{
	case Resource::Property::Type::INT:
	{
		auto data{ prop->GetInt() };
		if (ImGui::InputInt("##INSPECTOR/INT", &data))
			prop->Set(data);
		break;
	}
	case Resource::Property::Type::INT2:
	{
		auto data{ prop->GetInt2() };
		if (ImGui::InputInt2("##INSPECTOR/INT2", reinterpret_cast<int*>(&data)))
			prop->Set(data);
		break;
	}
	case Resource::Property::Type::FLOAT:
	{
		auto data{ prop->GetFloat() };
		if (ImGui::InputFloat("##INSPECTOR/FLOAT", &data))
			prop->Set(data);
		break;
	}
	case Resource::Property::Type::STRING:
	{
		auto data{ Util::wstou8s(prop->GetString()) };
		if (ImGui::InputTextMultiline("##INSPECTOR/STRING", &data))
			prop->Set(Util::u8stows(data));
		break;
	}
	case Resource::Property::Type::IMAGE:
	{
		if (!ImGui::Button("..."))
			break;

		std::wstring path(MAX_PATH, L'\0');
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"PNG Files (*.png)\0*.png\0";
		ofn.lpstrFile = path.data();
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
		ofn.lpstrDefExt = L"dat";
		if (!::GetOpenFileName(&ofn))
			break;

		std::ifstream file{ path, std::ios::binary };
		if (!file)
			break;

		file.seekg(0, std::ios::end);
		auto size{ static_cast<size_t>(file.tellg()) };
		file.seekg(0, std::ios::beg);

		std::unique_ptr<std::byte[]> buffer{ new std::byte[size]{} };
		file.read(reinterpret_cast<char*>(buffer.get()), size);

		auto image{ std::make_shared<Resource::Image>() };
		image->SetBuffer(buffer.release(), size);
		prop->Set(image);
		break;
	}
	}
}
