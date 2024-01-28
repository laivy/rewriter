#include "Stdafx.h"
#include "Include/Image.h"
#include "Include/Property.h"
#include "Include/Manager.h"

namespace Resource
{
	std::shared_ptr<Property> Manager::Get(const std::wstring& path)
	{
		std::wstring filePath{ path };
		std::wstring subPath{};

		size_t pos{ path.find(StringTable::DATA_FILE_EXT) };
		if (pos != std::wstring::npos)
		{
			filePath = path.substr(0, pos + 4);
			subPath = path.substr(pos + 4 + 1);
		}

#ifdef _DEBUG
		if (!filePath.ends_with(StringTable::DATA_FILE_EXT))
		{
			assert(false && "INVALID FILENAME");
			return nullptr;
		}
#endif

		if (m_resources.contains(filePath))
		{
			if (subPath.empty())
				return m_resources.at(filePath);
			else
				return m_resources.at(filePath)->Get(subPath);
		}

		auto root{ Resource::Load(filePath, subPath) };
		m_resources.emplace(filePath, root);
		return root;
	}

	void Manager::Unload(const std::wstring& path)
	{
		// 로드 된 모든 리소스 해제
		if (path.empty())
		{
			for (const auto& [_, p] : m_resources)
				p->Flush();

			std::erase_if(m_resources,
				[](const auto& r)
				{
					return r.second->m_children.empty();
				});

			return;
		}

		// path를 포함한 하위 리소스 해제
		size_t pos{ path.rfind(StringTable::DATA_PATH_SEPERATOR) };
		if (pos == std::wstring::npos && m_resources.contains(path)) // '/'가 없다는건 파일을 Unload 한다는 것
		{
			auto p{ m_resources.at(path) };
			p->Flush();
			if (p->m_children.empty())
				m_resources.erase(path);
		}
		else
		{
			std::wstring parent{ path.substr(0, pos) };
			std::wstring remain{ path.substr(pos + 1) };
			auto p{ Get(parent) };
			if (!p)
				return;

			auto c{ p->Get(remain) };
			if (!c)
				return;

			c->Flush();
			if (c->m_children.empty())
				std::erase(p->m_children, c);
		}
	}

	std::shared_ptr<Property> Get(const std::wstring& path)
	{
		if (auto m{ Manager::GetInstance() })
			return m->Get(path);
		return nullptr;
	}

	std::shared_ptr<Property> Get(const std::shared_ptr<Property>& prop, const std::wstring& path)
	{
		if (path.empty())
			return prop;

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(StringTable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return Get(prop->Get(name), remain);
		}

		if (const auto& child{ prop->Get(name) })
			return child;

		return nullptr;
	}

	void Unload(const std::wstring& path)
	{
		if (auto rm{ Manager::GetInstance() })
			rm->Unload(path);
	}
}