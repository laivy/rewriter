#include "Stdafx.h"
#include "App.h"
#include "Clipboard.h"
#include "Hierarchy.h"

namespace
{
	std::shared_ptr<Resource::Property> Clone(const std::shared_ptr<Resource::Property>& source)
	{
		auto clone{ std::make_shared<Resource::Property>() };
		clone->SetType(source->GetType());
		clone->SetName(source->GetName());
		switch (clone->GetType())
		{
		case Resource::Property::Type::Int:
		{
			clone->Set(source->GetInt());
			break;
		}
		case Resource::Property::Type::Int2:
		{
			clone->Set(source->GetInt2());
			break;
		}
		case Resource::Property::Type::Float:
		{
			clone->Set(source->GetFloat());
			break;
		}
		case Resource::Property::Type::String:
		{
			clone->Set(source->GetString());
			break;
		}
		case Resource::Property::Type::Sprite:
		{
			clone->Set(source->GetSprite());
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
			auto childClone{ Clone(child) };
			childClone->SetParent(clone);
			clone->Add(childClone);
		}
		return clone;
	}
}

void Clipboard::Copy(const std::vector<std::shared_ptr<Resource::Property>>& sources)
{
	m_sources.clear();
	for (const auto& source : sources)
		m_sources.push_back(Clone(source));
}

void Clipboard::Paste(const std::shared_ptr<Resource::Property>& destination) const
{
	for (const auto& source : m_sources)
	{
		auto clone{ Clone(source) };

		// 해당 이름의 프로퍼티가 이미 있을 경우 덮어 씀
		if (auto child{ destination->Get(clone->GetName()) })
			destination->Delete(child);

		clone->SetParent(destination);
		destination->Add(clone);
		App::OnPropertyAdd.Notify(clone);
	}

	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->OpenTree(destination);
}
