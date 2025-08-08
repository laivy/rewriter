#include "Stdafx.h"
#include "App.h"
#include "Delegates.h"
#include "FbxHandler.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/Util.h"

const std::map<Resource::Type, std::string_view> PROPERTY_TYPES
{
	{ Resource::Type::Folder, "Folder" },
	{ Resource::Type::Int, "Int" },
	{ Resource::Type::Int2, "Int2" },
	{ Resource::Type::Float, "Float" },
	{ Resource::Type::String, "String" },
	{ Resource::Type::Sprite, "Sprite" },
	//{ Resource::Type::Texture, "Texture" },
	{ Resource::Type::Model, "Model" }
};

Inspector::Inspector()
{
	Delegates::OnPropSelected.Register(this, std::bind_front(&Inspector::OnPropSelected, this));
	Delegates::OnPropDeleted.Register(this, std::bind_front(&Inspector::OnPropDeleted, this));
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

void Inspector::OnPropDeleted(const std::shared_ptr<Resource::Property>& prop)
{
	if (m_prop.lock() == prop)
		m_prop.reset();
}

void Inspector::OnPropSelected(const std::shared_ptr<Resource::Property>& prop)
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
	auto name{ Util::wstou8s(Resource::GetName(prop)) };
	if (ImGui::InputText("##INSPECTOR/NAME", &name, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		auto newName{ Util::u8stows(name) };
		auto parent{ Resource::GetParent(prop) };
		if (!parent || !Resource::Get(newName))
		{
			Resource::SetName(prop, newName);
			Delegates::OnPropModified.Notify(prop);
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

	if (ImGui::BeginCombo("##INSPECTOR/TYPE", PROPERTY_TYPES.at( Resource::GetType(prop)).data()))
	{
		for (const auto& [type, label] : PROPERTY_TYPES)
		{
			if (ImGui::Selectable(label.data()))
			{
				Resource::SetType(prop, type);
				Delegates::OnPropModified.Notify(prop);
			}
		}
		ImGui::EndCombo();
	}
}

void Inspector::RenderNodeValue(const std::shared_ptr<Resource::Property>& prop)
{
	auto type{ Resource::GetType(prop) };
	if (type == Resource::Type::Folder)
		return;

	ImGui::Text("Value");
	ImGui::SameLine(100);
	switch (type)
	{
	case Resource::Type::Int:
	{
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ Resource::GetInt(prop) };
		if (ImGui::InputInt("##INSPECTOR/INT", &data))
		{
			Resource::Set(prop, data);
			Delegates::OnPropModified.Notify(prop);
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
				Resource::Set(prop, data);
				Delegates::OnPropModified.Notify(prop);
			}
			ImGui::EndPopup();
		}
		break;
	}
	case Resource::Type::Int2:
	{
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ Resource::GetInt2(prop) };
		if (ImGui::InputInt2("##INSPECTOR/INT2", reinterpret_cast<int*>(&data)))
		{
			Resource::Set(prop, data);
			Delegates::OnPropModified.Notify(prop);
		}
		break;
	}
	case Resource::Type::Float:
	{
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ Resource::GetFloat(prop) };
		if (ImGui::InputFloat("##INSPECTOR/FLOAT", &data))
		{
			Resource::Set(prop, data);
			Delegates::OnPropModified.Notify(prop);
		}
		break;
	}
	case Resource::Type::String:
	{
		ImGui::SetNextItemWidth(-1.0f);
		auto data{ Util::wstou8s(Resource::GetString(prop)) };
		if (ImGui::InputTextMultiline("##INSPECTOR/STRING", &data))
		{
			Resource::Set(prop, Util::u8stows(data));
			Delegates::OnPropModified.Notify(prop);
		}
		break;
	}
	case Resource::Type::Sprite:
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
		Resource::Set(prop, sprite);
		Delegates::OnPropModified.Notify(prop);
		break;
	}
	/*
	case Resource::Type::Texture:
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
	*/
	case Resource::Type::Model:
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
		Resource::Set(prop, model);
		Delegates::OnPropModified.Notify(prop);
		break;
	}
	default:
		assert(false && "INVALID TYPE");
		break;
	}
}
