#include "Stdafx.h"
#include "App.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/Util.h"

const std::map<Resource::Property::Type, std::string> PROPERTY_TYPES
{
	{ Resource::Property::Type::Folder, "GROUP" },
	{ Resource::Property::Type::Int, "INT" },
	{ Resource::Property::Type::Int2, "INT2" },
	{ Resource::Property::Type::Float, "FLOAT" },
	{ Resource::Property::Type::String, "STRING" },
	{ Resource::Property::Type::Image, "IMAGE" }
};

Inspector::Inspector()
{
	App::OnPropertySelect.Register(this, std::bind_front(&Inspector::OnPropertySelect, this));
	App::OnPropertyDelete.Register(this, std::bind_front(&Inspector::OnPropertyDelete, this));
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
	if (IsRoot(prop))
	{
		auto& root{ GetRoot(prop) };
		ImGui::Text("Path");
		ImGui::SameLine(100);
		if (root.path.empty())
			ImGui::InputText("##PATH", "-", 1, ImGuiInputTextFlags_ReadOnly);
		else
			ImGui::InputText("##PATH", &Util::u8stou8s(root.path.u8string()), ImGuiInputTextFlags_ReadOnly);
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
				//if (auto parent{ Global::propInfo[prop].parent.lock() })
				//{
				//	auto it{ std::ranges::find_if(parent->GetChildren(), [&name](const auto& prop) { return prop->name == Util::u8stows(name); })};
				//	if (it != parent->children.cend())
				//		break;
				//}
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
	if (type == Resource::Property::Type::Folder)
		return;

	ImGui::Text("Value");
	ImGui::SameLine(100);
	switch (type)
	{
	case Resource::Property::Type::Int:
	{
		auto data{ prop->GetInt() };
		if (ImGui::InputInt("##INSPECTOR/INT", &data))
			prop->Set(data);
		break;
	}
	case Resource::Property::Type::Int2:
	{
		auto data{ prop->GetInt2() };
		if (ImGui::InputInt2("##INSPECTOR/INT2", reinterpret_cast<int*>(&data)))
			prop->Set(data);
		break;
	}
	case Resource::Property::Type::Float:
	{
		auto data{ prop->GetFloat() };
		if (ImGui::InputFloat("##INSPECTOR/FLOAT", &data))
			prop->Set(data);
		break;
	}
	case Resource::Property::Type::String:
	{
		auto data{ Util::wstou8s(prop->GetString()) };
		if (ImGui::InputTextMultiline("##INSPECTOR/STRING", &data))
			prop->Set(Util::u8stows(data));
		break;
	}
	case Resource::Property::Type::Image:
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
		auto size{ static_cast<uint32_t>(file.tellg()) };
		file.seekg(0, std::ios::beg);

		std::unique_ptr<std::byte[]> buffer{ new std::byte[size]{} };
		file.read(reinterpret_cast<char*>(buffer.get()), size);

		auto image{ std::make_shared<Resource::PNG>(buffer.release(), size) };
		prop->Set(image);
		break;
	}
	}
}
