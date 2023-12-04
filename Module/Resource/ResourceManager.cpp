#include "Stdafx.h"
#include "Common/StringTable.h"
#include "Include/Image.h"
#include "Include/Property.h"
#include "Include/ResourceManager.h"

namespace Resource
{
	std::shared_ptr<Property> ResourceManager::Get(const std::string& path)
	{
		std::string fileName{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			fileName = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (m_resources.contains(fileName))
		{
			if (remain.empty())
				return m_resources.at(fileName);
			else
				return m_resources.at(fileName)->Get(remain);
		}

		if (Load(path))
			return Get(path);
		return nullptr;
	}

	void ResourceManager::Unload(const std::string& path)
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
		size_t pos{ path.find_last_of('/') };
		if (pos == std::string::npos && m_resources.contains(path)) // '/'가 없다는건 파일을 Unload 한다는 것
		{
			auto p{ m_resources.at(path) };
			p->Flush();
			if (p->m_children.empty())
				m_resources.erase(path);
		}
		else
		{
			std::string parent{ path.substr(0, pos) };
			std::string remain{ path.substr(pos + 1) };
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

	bool ResourceManager::Load(const std::string& path)
	{
		std::string fileName{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			fileName = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		std::ifstream file{ StringTable::DATA_FOLDER_PATH + fileName, std::ios::binary };
		if (!file)
		{
			//assert(false && "CAN NOT OPEN FILE");
			return false;
		}

		int nodeCount{};
		file.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

		auto root{ std::make_shared<Property>() };
		root->m_type = Property::Type::GROUP;
		root->m_name = "root";
		root->m_children.reserve(nodeCount);

		for (int i = 0; i < nodeCount; ++i)
		{
			std::string temp{ remain };
			if (IsSkip(file, temp))
			{
				Skip(file);
				continue;
			}

			root->m_children.emplace_back(new Property);
			root->m_children.back()->Load(file, temp);
		}

		m_resources.emplace(fileName, root);
		return true;
	}

	bool IsSkip(std::ifstream& file, std::string& name)
	{
		if (name.empty())
			return false;

		bool isSkip{ false };

		char length{};
		file.read(reinterpret_cast<char*>(&length), sizeof(length));

		std::array<char, 128> buffer{};
		file.read(buffer.data(), length);

		// 직속 자식만 체크한다.
		std::string nodeName{ buffer.data(), static_cast<size_t>(length) };
		size_t pos{ name.find('/') };
		if (pos == std::string::npos)
			isSkip = nodeName != name;
		else
			isSkip = nodeName != name.substr(0, pos);

		if (!isSkip)
		{
			if (pos != std::string::npos)
				name = name.substr(pos + 1);
			else
				name.clear();
		}

		// 이 뒤에서 스트림으로부터 이름을 읽을 수 있도록 커서를 string만큼 뒤로 옮김
		file.seekg(-length - sizeof(length), std::ios::cur);

		return isSkip;
	}

	void Skip(std::ifstream& file)
	{
		char length{};
		file.read(reinterpret_cast<char*>(&length), sizeof(length));
		file.ignore(length);

		auto type{ Property::Type::GROUP };
		file.read(reinterpret_cast<char*>(&type), sizeof(type));

		switch (type)
		{
		case Property::Type::GROUP:
			break;
		case Property::Type::INT:
			file.ignore(sizeof(int));
			break;
		case Property::Type::INT2:
			file.ignore(sizeof(INT2));
			break;
		case Property::Type::FLOAT:
			file.ignore(sizeof(float));
			break;
		case Property::Type::STRING:
		{
			char length{};
			file.read(reinterpret_cast<char*>(&length), sizeof(length));
			file.ignore(length);
			break;
		}
		case Property::Type::IMAGE:
		{
			int length{};
			file.read(reinterpret_cast<char*>(&length), sizeof(length));
			file.ignore(length);
			break;
		}
		default:
			assert(false);
			break;
		}

		int nodeCount{};
		file.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));
		for (int i = 0; i < nodeCount; ++i)
			Skip(file);
	}

	std::shared_ptr<Property> Get(const std::string& path)
	{
		if (auto rm{ ResourceManager::GetInstance() })
			return rm->Get(path);
		return nullptr;
	}

	std::shared_ptr<Property> Get(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop;

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return Get(prop->Get(name), remain);
		}

		if (const auto& child{ prop->Get(name) })
			return child;

		return nullptr;
	}

	int GetInt(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetInt();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetInt(prop->Get(name), remain);
		}

		if (const auto& child{ prop->Get(name) })
			return child->GetInt();

		return 0;
	}

	INT2 GetInt2(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetInt2();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetInt2(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetInt2();

		return INT2{};
	}

	float GetFloat(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetFloat();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetFloat(prop->Get(name), remain);
		}

		if (const auto& child{ prop->Get(name) })
			return child->GetFloat();

		return 0.0f;
	}

	std::string GetString(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetString();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetString(prop->Get(name), remain);
		}

		if (const auto& child{ prop->Get(name) })
			return child->GetString();

		return "";
	}

	std::shared_ptr<Image> GetImage(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetImage();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetImage(prop->Get(name), remain);
		}

		if (const auto& child{ prop->Get(name) })
			return child->GetImage();

		return nullptr;
	}

	void Unload(const std::string& path)
	{
		if (auto rm{ ResourceManager::GetInstance() })
			rm->Unload(path);
	}
}