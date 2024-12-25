#include "Stdafx.h"
#include "Clipboard.h"
#include "Hierarchy.h"

namespace
{
	std::shared_ptr<Resource::Property> _Copy(const std::shared_ptr<Resource::Property>& source)
	{
		auto copy{ std::make_shared<Resource::Property>() };
		auto type{ source->GetType() };
		copy->SetType(type);
		copy->SetName(source->GetName());
		switch (type)
		{
		case Resource::Property::Type::Int:
		{
			copy->Set(source->GetInt());
			break;
		}
		case Resource::Property::Type::Int2:
		{
			copy->Set(source->GetInt2());
			break;
		}
		case Resource::Property::Type::Float:
		{
			copy->Set(source->GetFloat());
			break;
		}
		case Resource::Property::Type::String:
		{
			copy->Set(source->GetString());
			break;
		}
		case Resource::Property::Type::Sprite:
		{
			copy->Set(source->GetSprite());
			break;
		}
		case Resource::Property::Type::Texture:
		{
			// TODO
			break;
		}
		default:
			break;
		}
		for (const auto& [_, child] : *source)
		{
			auto copyChild{ _Copy(child) };
			copyChild->SetParent(copy);
			copy->Add(copyChild);
		}
		return copy;
	}
}

void Clipboard::Copy(const std::vector<std::shared_ptr<Resource::Property>>& targets)
{
	m_sources = targets;
}

void Clipboard::Paste(const std::shared_ptr<Resource::Property>& prop) const
{
	ImGui::PushID("CLIPBOARD");
	for (const auto& source : m_sources)
	{
		auto copy{ _Copy(source) };

		// 해당 이름의 프로퍼티가 이미 있을 경우 덮어씀
		if (auto child{ prop->Get(copy->GetName()) })
			prop->Delete(child);
		copy->SetParent(prop);
		prop->Add(copy);
	}
	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->OpenTree(prop);
	ImGui::PopID();
}

void Clipboard::Clear()
{
	m_sources.clear();
}
