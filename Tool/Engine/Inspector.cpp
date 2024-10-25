#include "Stdafx.h"
#include "App.h"
#include "Global.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/Util.h"

const std::map<Resource::Property::Type, const char*> PROPERTY_TYPES
{
	{ Resource::Property::Type::Folder, "Folder" },
	{ Resource::Property::Type::Int, "Int" },
	{ Resource::Property::Type::Int2, "Int2" },
	{ Resource::Property::Type::Float, "Float" },
	{ Resource::Property::Type::String, "String" },
	{ Resource::Property::Type::Sprite, "Sprite" }
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
		RenderNode();
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

void Inspector::RenderNode()
{
	auto prop{ m_prop.lock() };
	if (!prop)
		return;

	ImGui::AlignTextToFramePadding();
	ImGui::SeparatorText("Property Info");

	RenderNodeName(prop);
	RenderNodeType(prop);
	RenderNodeValue(prop);
}

void Inspector::RenderNodeName(const std::shared_ptr<Resource::Property>& prop)
{
	ImGui::Text("Name");
	ImGui::SameLine(100);
	auto name{ Util::wstou8s(prop->GetName()) };
	if (ImGui::InputText("##INSPECTOR/NAME", &name, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		auto newName{ Util::u8stows(name) };
		auto parent{ prop->GetParent() };
		auto child{ parent ? parent->Get(newName) : nullptr };
		if (!child)
			prop->SetName(newName);
	}
}

void Inspector::RenderNodeType(const std::shared_ptr<Resource::Property>& prop)
{
	ImGui::Text("Type");
	ImGui::SameLine(100);
	if (ImGui::BeginCombo("##INSPECTOR/TYPE", PROPERTY_TYPES.at(prop->GetType())))
	{
		for (const auto& [type, label] : PROPERTY_TYPES)
		{
			if (ImGui::Selectable(label))
				prop->SetType(type);
		}
		ImGui::EndCombo();
	}
}

void Inspector::RenderNodeValue(const std::shared_ptr<Resource::Property>& prop)
{
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
	case Resource::Property::Type::Sprite:
	{
		if (!ImGui::Button("..."))
			break;

		std::array<wchar_t, MAX_PATH> filePath{};
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"PNG Files (*.png)\0*.png\0";
		ofn.lpstrFile = filePath.data();
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
		ofn.lpstrDefExt = L"dat";
		if (!::GetOpenFileName(&ofn))
			break;

		std::ifstream file{ filePath.data(), std::ios::binary };
		if (!file)
			break;

		file.seekg(0, std::ios::end);
		auto size{ static_cast<uint32_t>(file.tellg()) };
		file.seekg(0, std::ios::beg);

		std::unique_ptr<std::byte[]> buffer{ new std::byte[size]{} };
		file.read(reinterpret_cast<char*>(buffer.get()), size);

		auto sprite{ std::make_shared<Resource::Sprite>(std::span{ buffer.release(), size }) };
		prop->Set(sprite);
		break;
	}
	default:
		assert(false && "INVALID TYPE");
		break;
	}
}
