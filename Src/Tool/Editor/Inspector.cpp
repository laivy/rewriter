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
		String
	};

	const std::unordered_map<Type, std::string_view> Types
	{
		{ Type::Folder, "폴더" },
		{ Type::Int32, "int32" },
		{ Type::Float, "float" },
		{ Type::String, "string" },
	};

	Type GetType(Resource::ID id)
	{
		if (Resource::GetInt(id))
			return Type::Int32;
		if (Resource::GetFloat(id))
			return Type::Float;
		if (Resource::GetString(id))
			return Type::String;
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
	bool isModified{ false };
	if (ImGui::Begin("속성"))
	{
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
		else
		{
			std::array<char, 2> dummy{ "-" };
			ImGui::BeginDisabled();
			ImGui::InputText("##value", dummy.data(), dummy.size(), ImGuiInputTextFlags_ReadOnly);
			ImGui::EndDisabled();
		}
	}
	ImGui::End();

	if (isModified)
		Delegates::OnPropertyModified.Broadcast(m_targetID);
}
