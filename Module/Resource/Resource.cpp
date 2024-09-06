#include "Stdafx.h"
#include "PNG.h"
#include "Property.h"
#include "Resource.h"
#if defined _CLIENT || defined _TOOL
#include <d2d1_3.h>
#endif

namespace
{
	std::shared_ptr<Resource::Property> Load(std::ifstream& file, std::wstring_view subPath)
	{
		std::function<std::shared_ptr<Resource::Property>(std::ifstream&, std::wstring_view)> loadChild =
			[](std::ifstream& file, std::wstring_view subPath) -> std::shared_ptr<Resource::Property>
			{
				auto prop{ std::make_shared<Resource::Property>() };

				// 이름
				int32_t length{ 0 };
				file.read(reinterpret_cast<char*>(&length), sizeof(length));

				std::array<wchar_t, 128> buffer{};
				file.read(reinterpret_cast<char*>(buffer.data()), length * sizeof(wchar_t));

				std::wstring name{ buffer.data(), static_cast<size_t>(length) };
				prop->SetName(name);

				// 타입
				Resource::Property::Type type{};
				file.read(reinterpret_cast<char*>(&type), sizeof(type));
				prop->SetType(type);

				// 데이터
				switch (type)
				{
				case Resource::Property::Type::FOLDER:
				{
					break;
				}
				case Resource::Property::Type::INT:
				{
					int32_t value{};
					file.read(reinterpret_cast<char*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Resource::Property::Type::INT2:
				{
					INT2 value{};
					file.read(reinterpret_cast<char*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Resource::Property::Type::FLOAT:
				{
					float value{};
					file.read(reinterpret_cast<char*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Resource::Property::Type::STRING:
				{
					int32_t len{ 0 };
					file.read(reinterpret_cast<char*>(&len), sizeof(len));

					buffer.fill(0);
					file.read(reinterpret_cast<char*>(buffer.data()), len * sizeof(wchar_t));

					std::wstring value{ buffer.data(), static_cast<size_t>(len) };
					prop->Set(value);
					break;
				}
				case Resource::Property::Type::PNG:
				{
					int32_t size{};
					file.read(reinterpret_cast<char*>(&size), sizeof(size));
#if defined _CLIENT || defined _TOOL
					std::unique_ptr<std::byte[]> buffer{ new std::byte[size]{} };
					file.read(reinterpret_cast<char*>(buffer.get()), size);

					auto value{ std::make_shared<Resource::PNG>(buffer.get(), static_cast<size_t>(size)) };
					prop->Set(value);
#else // defined _CLIENT || defined _TOOL
					file.ignore(size);
#endif 
					break;
				}
				case Resource::Property::Type::DDS:
				{
					// TODO
					break;
				}
				default:
					assert(false && "INVALID PROPERTY TYPE");
					return nullptr;
				}

				return prop;
			};

		auto root{ std::make_shared<Resource::Property>() };
		root->SetType(Resource::Property::Type::FOLDER);
		root->SetName(L"Root");

		int32_t childCount{ 0 };
		file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
		for (int32_t i = 0; i < childCount; ++i)
			root->Add(loadChild(file, subPath));
		return root;
	}
};

namespace Resource
{
	std::map<std::wstring, std::shared_ptr<Property>> g_resources;

#if defined _CLIENT || defined _TOOL
	ComPtr<ID2D1DeviceContext2> d2dContext;

	DLL_API void Initialize(const ComPtr<ID2D1DeviceContext2>& d2dContext)
	{
		Resource::d2dContext = d2dContext;
	}
#endif // _CLIENT || _TOOL

	DLL_API std::shared_ptr<Property> Get(std::wstring_view path)
	{
		std::wstring filePath{ path };
		std::wstring_view subPath{};

		// 파라미터로부터 파일 이름과
		size_t pos{ path.find(Stringtable::DATA_FILE_EXT) };
		if (pos != std::wstring_view::npos)
		{
			constexpr auto EXT_LENGTH{ std::char_traits<wchar_t>::length(Stringtable::DATA_FILE_EXT) };

			filePath = path.substr(0, pos + EXT_LENGTH);
			if (path.size() > pos + EXT_LENGTH + 1)
				subPath = path.substr(pos + EXT_LENGTH + 1);
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
#ifdef _TOOL
		std::ifstream file{ filePath, std::ios::binary };
#else
		std::ifstream file{ Stringtable::DATA_FOLDER_PATH + filePath, std::ios::binary };
#endif
		if (!file)
		{
			assert(false && "CAN NOT FIND DATA FILE");
			return nullptr;
		}

		auto root{ Load(file, subPath) };
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
