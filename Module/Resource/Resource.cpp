#include "Stdafx.h"
#include "Global.h"
#include "Property.h"
#include "Resource.h"
#include "Sprite.h"

namespace
{
	using namespace Resource;

#ifdef _TOOL
	bool _Save(std::ofstream& file, const std::shared_ptr<Property>& prop)
	{
		// 이름
		auto name{ prop->GetName() };
		auto length{ static_cast<uint16_t>(name.size()) };
		file.write(reinterpret_cast<const char*>(&length), sizeof(length));
		file.write(reinterpret_cast<const char*>(name.data()), name.size() * sizeof(std::wstring::value_type));

		// 타입
		auto type{ prop->GetType() };
		file.write(reinterpret_cast<const char*>(&type), sizeof(type));

		// 데이터
		switch (type)
		{
		case Property::Type::Folder:
		{
			break;
		}
		case Property::Type::Int:
		{
			auto data{ prop->GetInt() };
			file.write(reinterpret_cast<const char*>(&data), sizeof(data));
			break;
		}
		case Property::Type::Int2:
		{
			auto data{ prop->GetInt2() };
			file.write(reinterpret_cast<const char*>(&data), sizeof(data));
			break;
		}
		case Property::Type::Float:
		{
			auto data{ prop->GetFloat() };
			file.write(reinterpret_cast<const char*>(&data), sizeof(data));
			break;
		}
		case Property::Type::String:
		{
			auto data{ prop->GetString() };
			auto length{ static_cast<uint16_t>(data.size()) };
			file.write(reinterpret_cast<const char*>(&length), sizeof(length));
			file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(std::wstring::value_type));
			break;
		}
		case Property::Type::Sprite:
		{
			auto data{ prop->GetSprite() };
			auto length{ data->GetBinarySize() };
			auto binary{ data->GetBinary() };
			file.write(reinterpret_cast<const char*>(&length), sizeof(length));
			file.write(reinterpret_cast<const char*>(binary), length);
			break;
		}
		default:
			assert(false && "INVALID PROPERTY TYPE");
			return false;
		}

		// 자식
		auto children{ prop->GetChildren() };
		auto count{ static_cast<uint16_t>(children.size()) };
		file.write(reinterpret_cast<const char*>(&count), sizeof(count));
		for (const auto& child : children)
		{
			if (!_Save(file, child))
				return false;
		}
		return true;
	}
#endif

	std::shared_ptr<Property> _Load(std::ifstream& file, std::wstring_view subPath)
	{
		auto prop{ std::make_shared<Property>() };

		// 이름
		uint16_t length{};
		file.read(reinterpret_cast<char*>(&length), sizeof(length));

		std::wstring name(length, L'\0');
		file.read(reinterpret_cast<char*>(name.data()), length * sizeof(std::wstring::value_type));
		prop->SetName(name);

		// 타입
		auto type{ Property::Type::Folder };
		file.read(reinterpret_cast<char*>(&type), sizeof(type));
		prop->SetType(type);

		// 데이터
		switch (type)
		{
		case Property::Type::Folder:
		{
			break;
		}
		case Property::Type::Int:
		{
			int32_t data{};
			file.read(reinterpret_cast<char*>(&data), sizeof(data));
			prop->Set(data);
			break;
		}
		case Property::Type::Int2:
		{
			INT2 data{};
			file.read(reinterpret_cast<char*>(&data), sizeof(data));
			prop->Set(data);
			break;
		}
		case Property::Type::Float:
		{
			float data{};
			file.read(reinterpret_cast<char*>(&data), sizeof(data));
			prop->Set(data);
			break;
		}
		case Property::Type::String:
		{
			uint16_t length{};
			file.read(reinterpret_cast<char*>(&length), sizeof(length));

			std::wstring data(length, L'\0');
			file.read(reinterpret_cast<char*>(data.data()), length * sizeof(std::wstring::value_type));
			prop->Set(data);
			break;
		}
		case Property::Type::Sprite:
		{
			uint32_t length{};
			file.read(reinterpret_cast<char*>(&length), sizeof(length));
#ifdef _SERVER
			file.ignore(length);
#else
			std::unique_ptr<std::byte[]> binary{ new std::byte[length]{} };
			file.read(reinterpret_cast<char*>(binary.get()), length);

			auto data{ std::make_shared<Sprite>(std::span{ binary.get(), length }) };
			prop->Set(data);
#endif
			break;
		}
		default:
			assert(false && "INVALID PROPERTY TYPE");
			return nullptr;
		}

		// 자식
		uint16_t count{};
		file.read(reinterpret_cast<char*>(&count), sizeof(count));
		for (uint16_t i{ 0 }; i < count; ++i)
		{
			auto child{ _Load(file, subPath) };
			prop->Add(child);
		}
		return prop;
	}

	std::shared_ptr<Property> Load(std::wstring_view filePath, std::wstring_view subPath)
	{
#ifdef _TOOL
		std::ifstream file{ filePath.data(), std::ios::binary };
#else
		std::ifstream file{ Stringtable::DATA_FOLDER_PATH + std::wstring{ filePath }, std::ios::binary };
#endif
		if (!file)
		{
			assert(false && "CAN NOT LOAD DATA FILE");
			return nullptr;
		}

		auto root{ _Load(file, subPath) };
		if (!subPath.empty())
			return root->Get(subPath);
		return root;
	}
};

namespace Resource
{
#if defined _CLIENT || defined _TOOL
	DLL_API void Initialize(const ComPtr<ID2D1DeviceContext2>& d2dContext)
	{
		g_d2dContext = d2dContext;
		if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
			assert(false && "COINITIALIZE FAIL");
	}
#endif

	DLL_API void CleanUp()
	{
		g_resources.clear();
#if defined _CLIENT || defined _TOOL
		g_d2dContext.Reset();
		::CoUninitialize();
#endif
	}

#ifdef _TOOL
	DLL_API bool Save(const std::shared_ptr<Property>& prop, std::wstring_view path)
	{
		prop->SetName(Stringtable::ROOT);

		std::ofstream file{ path.data(), std::ios::binary };
		if (!file)
		{
			assert(false && "CAN NOT SAVE DATA FILE");
			return false;
		}
		return _Save(file, prop);
	}
#endif

	DLL_API std::shared_ptr<Property> Get(std::wstring_view path)
	{
		std::wstring filePath{ path };
		std::wstring_view subPath{};

		// 파라미터를 파일 경로와 세부 경로로 나눔
		size_t pos{ path.find(Stringtable::DATA_FILE_EXT) };
		if (pos != std::wstring_view::npos)
		{
			constexpr std::wstring_view EXTENSION{ Stringtable::DATA_FILE_EXT };

			filePath = path.substr(0, pos + EXTENSION.size());
			if (path.size() > pos + EXTENSION.size() + 1)
				subPath = path.substr(pos + EXTENSION.size() + 1);
		}

#ifdef _DEBUG
		if (!filePath.ends_with(Stringtable::DATA_FILE_EXT))
		{
			assert(false && "INVALID FILENAME");
			return nullptr;
		}
#endif

		// 이미 로드된 데이터인지 확인
		if (g_resources.contains(filePath))
		{
			if (subPath.empty())
				return g_resources[filePath];
			return g_resources[filePath]->Get(subPath);
		}

		// 로드
		auto root{ Load(filePath, subPath) };
		if (!root)
			return nullptr;

		g_resources.emplace(filePath, root);
		if (subPath.empty())
			return root;
		return root->Get(subPath);
	}

	DLL_API void Unload(std::wstring_view path)
	{
		// 모든 리소스 해제
		if (path.empty())
		{
			g_resources.clear();
			return;
		}

		size_t pos{ path.rfind(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos == std::wstring_view::npos) // '/'가 없다는건 파일을 Unload 한다는 것
		{
			g_resources.erase(path.data());
			return;
		}

		auto parent{ Get(path.substr(0, pos)) };
		auto target{ path.substr(pos + 1) };
		std::erase_if(parent->GetChildren(), [target](const auto& prop) { return prop->GetName() == target; });
	}
}
