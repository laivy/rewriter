#include "Pch.h"
#include "Delegates.h"
#include "Inspector.h"
#include <Common/Util.h>

namespace
{
	enum class Type
	{
		Folder,
		Int32,
		Float,
		String,
		Sprite
	};

	const std::unordered_map<Type, std::string_view> Types
	{
		{ Type::Folder, "폴더" },
		{ Type::Int32, "int32" },
		{ Type::Float, "float" },
		{ Type::String, "string" },
		{ Type::Sprite, "sprite" }
	};

	Type GetType(Resource::ID id)
	{
		if (Resource::GetInt(id))
			return Type::Int32;
		if (Resource::GetFloat(id))
			return Type::Float;
		if (Resource::GetString(id))
			return Type::String;
		if (Resource::GetSprite(id))
			return Type::Sprite;
		return Type::Folder;
	}
}

Inspector::Inspector() :
	m_targetID{ Resource::InvalidID }
{
	Delegates::OnPropertyDeleted.Bind(this, [this](Resource::ID id)
	{
		if (m_targetID == id)
			m_targetID = Resource::InvalidID;
	});
	Delegates::OnPropertySelected.Bind(this, [this](Resource::ID id)
	{
		m_targetID = id;
	});
}

void Inspector::Update(float deltaSeconds)
{
}

void Inspector::Render()
{
	if (!ImGui::Begin("속성"))
	{
		ImGui::End();
		return;
	}

	bool isModified{ false };

	const Resource::ID id{ m_targetID };
	if (id == Resource::InvalidID)
	{
		ImGui::End();
		return;
	}

	const Type type{ GetType(id) };
	ImGui::AlignTextToFramePadding();

	const float labelWidth{ ImGui::CalcTextSize("타입").x + ImGui::GetStyle().ItemSpacing.x * 2.0f };
	ImGui::TextUnformatted("이름");
	ImGui::SameLine(labelWidth);
	if (auto name{ Resource::GetName(id) })
	{
		ImGui::SetNextItemWidth(-ImGui::GetStyle().WindowPadding.x);
		std::string str{ Util::ToU8String(*name) };
		if (ImGui::InputText("##name", &str, ImGuiInputTextFlags_EnterReturnsTrue))
			isModified = Resource::SetName(id, Util::ToWString(str));
	}

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted("타입");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(-ImGui::GetStyle().WindowPadding.x);
	if (ImGui::BeginCombo("##type", Types.at(type).data()))
	{
		for (const auto& [key, value] : Types)
		{
			if (!ImGui::Selectable(value.data(), key == type))
				continue;

			switch (key)
			{
			case Type::Folder:
				Resource::Set(id, std::monostate{});
				break;
			case Type::Int32:
				Resource::Set(id, 0);
				break;
			case Type::Float:
				Resource::Set(id, 0.0f);
				break;
			case Type::String:
				Resource::Set(id, L"");
				break;
			case Type::Sprite:
				Resource::Set(id, Resource::Sprite{});
				break;
			default:
				break;
			}
			isModified = true;
		}
		ImGui::EndCombo();
	}

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted("값");
	ImGui::SameLine(labelWidth);
	ImGui::SetNextItemWidth(-ImGui::GetStyle().WindowPadding.x);
	if (auto value{ Resource::GetInt(id) })
	{
		if (ImGui::InputInt("##value", &(*value)))
			isModified = Resource::Set(id, *value);
	}
	else if (auto value{ Resource::GetFloat(id) })
	{
		if (ImGui::InputFloat("##value", &(*value), 0.0f, 0.0f, "%.6f"))
			isModified = Resource::Set(id, *value);
	}
	else if (auto value{ Resource::GetString(id) })
	{
		std::string str{ Util::ToU8String(*value) };
		if (ImGui::InputText("##value", &str))
			isModified = Resource::Set(id, Util::ToWString(str));
	}
	else if (auto value{ Resource::GetSprite(id) })
	{
		if (ImGui::Button("파일 열기(*.png)", ImVec2{ -ImGui::GetStyle().WindowPadding.x, 0.0f }))
		{
			Graphics::ImGui::FileDialog::Open(
				"OpenSpriteFile",
				Graphics::ImGui::FileDialog::Type::Open,
				Graphics::ImGui::FileDialog::Target::File,
				{ L".png" }
			);
		}

		static float scale{ 100.0f };
		const auto& style{ ImGui::GetStyle() };
		ImVec2 region{ ImGui::GetContentRegionAvail() - style.WindowPadding };
		region.y -= ImGui::GetFrameHeight();
		if (ImGui::BeginChild("preview", region, ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImTextureID textureID{ Graphics::ImGui::GetTexture(id) };
			ImVec2 imageSize{ Graphics::ImGui::GetTextureSize(textureID) };
			imageSize *= scale / 100.0f;

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{});
			if (imageSize.x < region.x && imageSize.y < region.y)
			{
				ImGui::Dummy(ImVec2{ 0.0f, (region.y - imageSize.y) / 2.0f });
				ImGui::Dummy(ImVec2{ (region.x - imageSize.x) / 2.0f, 0.0f });
				ImGui::SameLine();

				const ImVec2 cursor{ ImGui::GetCursorScreenPos() };
				auto drawList{ ImGui::GetWindowDrawList() };
				drawList->AddRect(cursor, cursor + imageSize, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]));
			}
			else if (imageSize.x < region.x - style.ScrollbarSize)
			{
				ImGui::Dummy(ImVec2{ (region.x - style.ScrollbarSize - imageSize.x) / 2.0f, 0.0f });
				ImGui::SameLine();

				const ImVec2 cursor{ ImGui::GetCursorScreenPos() };
				auto drawList{ ImGui::GetWindowDrawList() };
				drawList->AddLine(cursor, cursor + ImVec2{ 0.0f, imageSize.y }, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]));
				drawList->AddLine(cursor + ImVec2{ imageSize.x, 0.0f }, cursor + imageSize, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]));
			}
			else if (imageSize.y < region.y - style.ScrollbarSize)
			{
				ImGui::Dummy(ImVec2{ 0.0f, (region.y - style.ScrollbarSize - imageSize.y) / 2.0f });

				const ImVec2 cursor{ ImGui::GetCursorScreenPos() };
				auto drawList{ ImGui::GetWindowDrawList() };
				drawList->AddLine(cursor, cursor + ImVec2{ imageSize.x, 0.0f }, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]));
				drawList->AddLine(cursor + ImVec2{ 0.0f, imageSize.y }, cursor + imageSize, ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]));
			}
			if (textureID == 0)
				ImGui::Dummy(imageSize);
			else
				ImGui::Image(textureID, imageSize);
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();

		if (ImGui::BeginChild("scale", ImVec2{ -style.WindowPadding.x, ImGui::GetFrameHeight() }))
		{
			ImTextureID textureID{ Graphics::ImGui::GetTexture(id) };
			const ImVec2 size{ Graphics::ImGui::GetTextureSize(textureID) };
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("%.0f x %.0f", size.x, size.y);
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{});
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::DragFloat("##scale", &scale, 1.0f, 0.1f, 500.0f, "%.0f%%");
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
	else
	{
		std::array<char, 2> dummy{ "-" };
		ImGui::BeginDisabled();
		ImGui::InputText("##value", dummy.data(), dummy.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::EndDisabled();
	}
	ImGui::End();

	if (auto path{ Graphics::ImGui::FileDialog::Render("OpenSpriteFile") })
	{
		std::ifstream file{ *path, std::ios::binary };
		auto buffer{ std::make_shared<std::vector<char>>() };
		buffer->assign(std::istreambuf_iterator<char>(file), {});
		isModified = Resource::Set(id, Resource::Sprite{ .binary = std::move(buffer) });
		Graphics::ImGui::CreateTexture(id);
	}

	if (isModified)
		Delegates::OnPropertyModified.Broadcast(id);
}
