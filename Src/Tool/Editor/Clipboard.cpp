#include "Pch.h"
#include "App.h"
#include "Clipboard.h"
#include "Delegates.h"
#include "Hierarchy.h"

void Clipboard::Copy(const std::vector<std::shared_ptr<Resource::Property>>& sources)
{
	m_sources.clear();
	for (const auto& source : sources)
		m_sources.push_back(Resource::Clone(source));
}

void Clipboard::Paste(const std::shared_ptr<Resource::Property>& destination) const
{
	/* TODO
	for (const auto& source : m_sources)
	{
		auto clone{ Resource::Clone(source) };

		// 해당 이름의 프로퍼티가 이미 있을 경우 덮어 씀
		if (auto child{ destination->Get(clone->GetName()) })
			destination->Delete(child);

		clone->SetParent(destination);
		destination->Add(clone);
		Delegates::OnPropertyAdded.Notify(clone);
	}

	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->OpenTree(destination);
	*/
}
