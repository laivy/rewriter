#include "Stdafx.h"
#include "Image.h"
#include "Manager.h"
#include "Property.h"

namespace Resource
{
	bool IsSkip(std::ifstream& file, std::wstring& name)
	{
		if (name.empty())
			return false;

		auto streamPos{ file.tellg() }; // 현재 파일 포인터 위치 저장
		bool isSkip{ false };

		int32_t length{};
		file.read(reinterpret_cast<char*>(&length), sizeof(length));

		std::array<wchar_t, 128> buffer{};
		file.read(reinterpret_cast<char*>(buffer.data()), length * sizeof(wchar_t));

		// 직속 자식만 체크한다.
		std::wstring nodeName{ buffer.data(), static_cast<size_t>(length) };
		size_t pos{ name.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos == std::wstring::npos)
			isSkip = nodeName != name;
		else
			isSkip = nodeName != name.substr(0, pos);

		if (!isSkip)
		{
			if (pos == std::wstring::npos)
				name.clear();
			else
				name = name.substr(pos + 1);
		}

		// 읽기 전 위치로 옮김
		file.seekg(streamPos);
		return isSkip;
	}

	void Skip(std::ifstream& file)
	{
		int32_t length{};
		file.read(reinterpret_cast<char*>(&length), sizeof(length));
		file.ignore(length * sizeof(wchar_t));

		Property::Type type{};
		file.read(reinterpret_cast<char*>(&type), sizeof(type));

		switch (type)
		{
		case Property::Type::FOLDER:
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

	std::shared_ptr<Property> Manager::Get(std::wstring_view path)
	{
		std::wstring filePath{ path };
		std::wstring subPath{};

		size_t pos{ path.find(Stringtable::DATA_FILE_EXT) };
		if (pos != std::wstring::npos)
		{
			const size_t extLength{ std::wcslen(Stringtable::DATA_FILE_EXT) };
			filePath = path.substr(0, pos + extLength);

			auto l = path.size();
			if (path.size() > pos + extLength + 1)
				subPath = path.substr(pos + extLength + 1);
		}

#ifdef _DEBUG
		if (!filePath.ends_with(Stringtable::DATA_FILE_EXT))
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

		if (subPath.empty())
			return root;
		return root->Get(subPath);
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
					return r.second->children.empty();
				});

			return;
		}

		// path를 포함한 하위 리소스 해제
		size_t pos{ path.rfind(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos == std::wstring::npos && m_resources.contains(path)) // '/'가 없다는건 파일을 Unload 한다는 것
		{
			auto p{ m_resources.at(path) };
			p->Flush();
			if (p->children.empty())
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
			if (c->children.empty())
				std::erase(p->children, c);
		}
	}

	std::shared_ptr<Property> Get(std::wstring_view path)
	{
		if (auto m{ Manager::GetInstance() })
			return m->Get(path);
		return nullptr;
	}

	std::shared_ptr<Property> Load(const std::filesystem::path& path, const std::wstring& subPath)
	{
		// 서버, 클라에서는 상대 경로, 툴에서는 절대 경로를 사용함
		std::filesystem::path p{ path };
		if (p.is_relative())
			p = Stringtable::DATA_FOLDER_PATH + p.wstring();

		std::ifstream file{ p, std::ios::binary };
		if (!file)
		{
#ifdef _DEBUG
			assert(false && "CAN NOT FIND DATA FILE");
#endif
			return nullptr;
		}

		std::function<void(const std::shared_ptr<Property>&, std::wstring&)> lambda = [&](const std::shared_ptr<Property>& prop, std::wstring& path)
			{
				// 이름
				int32_t length{};
				file.read(reinterpret_cast<char*>(&length), sizeof(length));

				std::array<wchar_t, 128> buffer{};
				file.read(reinterpret_cast<char*>(buffer.data()), length * sizeof(wchar_t));

				std::wstring name{ buffer.data(), static_cast<size_t>(length) };
				prop->SetName(name);

				// 타입
				Property::Type type{};
				file.read(reinterpret_cast<char*>(&type), sizeof(type));
				prop->SetType(type);

				// 데이터
				switch (type)
				{
				case Property::Type::FOLDER:
					break;
				case Property::Type::INT:
				{
					int value{};
					file.read(reinterpret_cast<char*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Property::Type::INT2:
				{
					INT2 value{};
					file.read(reinterpret_cast<char*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Property::Type::FLOAT:
				{
					float value{};
					file.read(reinterpret_cast<char*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Property::Type::STRING:
				{
					length = 0;
					file.read(reinterpret_cast<char*>(&length), sizeof(length));

					buffer.fill(0);
					file.read(reinterpret_cast<char*>(buffer.data()), length * sizeof(wchar_t));

					prop->Set(std::wstring{ buffer.data(), static_cast<size_t>(length) });
					break;
				}
				case Property::Type::IMAGE:
				{
					int32_t size{};
					file.read(reinterpret_cast<char*>(&size), sizeof(size));

					std::unique_ptr<std::byte[]> buffer{ new std::byte[size]{} };
					file.read(reinterpret_cast<char*>(buffer.get()), size);

					auto image{ std::make_shared<Image>() };
					image->SetBuffer(buffer.release(), size);
					prop->Set(image);
					break;
				}
				default:
					assert(false && "INVALID PROPERTY TYPE");
					break;
				}

				// 자식
				int32_t count{};
				file.read(reinterpret_cast<char*>(&count), sizeof(count));
				for (int32_t i = 0; i < count; ++i)
				{
					std::wstring temp{ path };
					if (IsSkip(file, temp))
					{
						Skip(file);
						continue;
					}

					auto child{ std::make_shared<Property>() };
					lambda(child, temp);
					prop->Add(child);
				}
			};

		auto root{ std::make_shared<Property>() };
		root->SetType(Property::Type::FOLDER);
		root->SetName(L"Root");

		int32_t count{};
		file.read(reinterpret_cast<char*>(&count), sizeof(count));
		for (int i = 0; i < count; ++i)
		{
			std::wstring temp{ subPath };
			if (IsSkip(file, temp))
			{
				Skip(file);
				continue;
			}

			auto child{ std::make_shared<Property>() };
			lambda(child, temp);
			root->Add(child);
		}

		return root;
	}

	void Unload(const std::wstring& path)
	{
		if (auto rm{ Manager::GetInstance() })
			rm->Unload(path);
	}
}