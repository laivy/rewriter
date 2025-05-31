#include "Stdafx.h"
#include "App.h"
#include "FbxHandler.h"
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
	{ Resource::Property::Type::Sprite, "Sprite" },
	{ Resource::Property::Type::Texture, "Texture" },
	{ Resource::Property::Type::Model, "Model" }
};

Inspector::Inspector()
{
	App::OnPropertySelected.Register(this, std::bind_front(&Inspector::OnPropertySelected, this));
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

void Inspector::OnPropertyDelete(const std::shared_ptr<Resource::Property>& prop)
{
	if (m_prop.lock() == prop)
		m_prop.reset();
}

void Inspector::OnPropertySelected(const std::shared_ptr<Resource::Property>& prop)
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
	ImGui::SetNextItemWidth(-1.0f);
	auto name{ Util::wstou8s(prop->GetName()) };
	if (ImGui::InputText("##INSPECTOR/NAME", &name, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		auto newName{ Util::u8stows(name) };
		auto parent{ prop->GetParent() };
		if (!parent || !parent->Get(newName))
		{
			prop->SetName(newName);
			App::OnPropertyModified.Notify(prop);
		}
	}
}

void Inspector::RenderNodeType(const std::shared_ptr<Resource::Property>& prop)
{
	ImGui::Text("Type");
	ImGui::SameLine(100);
	ImGui::SetNextItemWidth(-1.0f);
	if (auto hierarchy{ Hierarchy::GetInstance() }; hierarchy && hierarchy->IsRoot(prop))
	{
		if (ImGui::BeginCombo("##INSPECTOR/TYPE", "File"))
		{
			ImGui::Selectable("File");
			ImGui::EndCombo();
		}
		return;
	}

	if (ImGui::BeginCombo("##INSPECTOR/TYPE", PROPERTY_TYPES.at(prop->GetType())))
	{
		for (const auto& [type, label] : PROPERTY_TYPES)
		{
			if (ImGui::Selectable(label))
			{
				prop->SetType(type);
				App::OnPropertyModified.Notify(prop);
			}
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
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ prop->GetInt() };
		if (ImGui::InputInt("##INSPECTOR/INT", &data))
		{
			prop->Set(data);
			App::OnPropertyModified.Notify(prop);
		}

		auto cursorPosition{ ImGui::GetCursorScreenPos() };
		if (ImGui::BeginPopupContextItem("##INSPECTOR/INT/COLOR"))
		{
			// ImGui는 0xAABBGGRR, 데이터는 0xAARRGGBB
			ImVec4 color{};
			color.x = ((data & 0x00FF0000) >> 16) / static_cast<float>(0xFF);
			color.y = ((data & 0x0000FF00) >> 8) / static_cast<float>(0xFF);
			color.z = ((data & 0x000000FF) >> 0) / static_cast<float>(0xFF);
			color.w = ((data & 0xFF000000) >> 24) / static_cast<float>(0xFF);

			ImGui::SetWindowPos(cursorPosition);
			if (ImGui::ColorPicker4("##NONE", reinterpret_cast<float*>(&color), ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_Uint8))
			{
				auto r{ static_cast<uint32_t>(color.x * 255.0f) & 0xFF };
				auto g{ static_cast<uint32_t>(color.y * 255.0f) & 0xFF };
				auto b{ static_cast<uint32_t>(color.z * 255.0f) & 0xFF };
				auto a{ static_cast<uint32_t>(color.w * 255.0f) & 0xFF };
				data = (a << 24) | (r << 16) | (g << 8) | b;
				prop->Set(data);
				App::OnPropertyModified.Notify(prop);
			}
			ImGui::EndPopup();
		}
		break;
	}
	case Resource::Property::Type::Int2:
	{
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ prop->GetInt2() };
		if (ImGui::InputInt2("##INSPECTOR/INT2", reinterpret_cast<int*>(&data)))
		{
			prop->Set(data);
			App::OnPropertyModified.Notify(prop);
		}
		break;
	}
	case Resource::Property::Type::Float:
	{
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ prop->GetFloat() };
		if (ImGui::InputFloat("##INSPECTOR/FLOAT", &data))
		{
			prop->Set(data);
			App::OnPropertyModified.Notify(prop);
		}
		break;
	}
	case Resource::Property::Type::String:
	{
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ Util::wstou8s(prop->GetString()) };
		if (ImGui::InputTextMultiline("##INSPECTOR/STRING", &data))
		{
			prop->Set(Util::u8stows(data));
			App::OnPropertyModified.Notify(prop);
		}
		break;
	}
	case Resource::Property::Type::Sprite:
	{
		ImGui::SetNextItemWidth(50.0f);
		if (!ImGui::Button("..."))
			break;

		std::array<wchar_t, MAX_PATH> filePath{};
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"PNG Files (*.png)\0*.png\0";
		ofn.lpstrFile = filePath.data();
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
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

		std::span<std::byte> binary{ buffer.release(), size };
		auto sprite{ Graphics::D2D::LoadSprite(binary) };
		prop->Set(sprite);
		App::OnPropertyModified.Notify(prop);
		break;
	}
	case Resource::Property::Type::Texture:
	{
		ImGui::SetNextItemWidth(50.0f);
		if (!ImGui::Button("..."))
			break;

		std::array<wchar_t, MAX_PATH> filePath{};
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"DDS Files (*.dds)\0*.dds\0";
		ofn.lpstrFile = filePath.data();
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
		if (!::GetOpenFileName(&ofn))
			break;

		std::ifstream file{ filePath.data(), std::ios::binary };
		if (!file)
			break;

		break;
	}
	case Resource::Property::Type::Model:
	{
		ImGui::SetNextItemWidth(50.0f);
		if (!ImGui::Button("..."))
			break;

		std::array<wchar_t, MAX_PATH> filePath{};
		OPENFILENAME ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"FBX Files (*.fbx)\0*.fbx\0";
		ofn.lpstrFile = filePath.data();
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER;
		if (!::GetOpenFileName(&ofn))
			break;

		auto fbxHandler{ FbxHandler::GetInstance() };
		if (!fbxHandler)
			break;

		auto model{ fbxHandler->Load(filePath.data()) };
		prop->Set(model);
		App::OnPropertyModified.Notify(prop);
		break;
	}
	default:
		assert(false && "INVALID TYPE");
		break;
	}
}
