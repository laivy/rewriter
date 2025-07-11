#include "Stdafx.h"
#include "Model.h"
#include "Property.h"
#include "Resource.h"
#include "Sprite.h"

namespace Resource
{
	std::map<std::wstring, std::shared_ptr<Property>> g_resources;
	std::wstring g_mountPath;
#if defined _CLIENT || defined _TOOL
	std::function<std::shared_ptr<Sprite>(std::span<std::byte>)> g_loadSprite;
	std::function<std::shared_ptr<Texture>(std::span<std::byte>)> g_loadTexture;
	std::function<std::shared_ptr<Model>(std::span<std::byte>)> g_loadModel;
#endif

#if defined _CLIENT || defined _TOOL
	DLL_API void Initialize(
		std::wstring_view mountPath,
		const std::function<std::shared_ptr<Sprite>(std::span<std::byte>)>& loadSprite,
		const std::function<std::shared_ptr<Texture>(std::span<std::byte>)>& loadTexture,
		const std::function<std::shared_ptr<Model>(std::span<std::byte>)>& loadModel
	)
	{
		g_mountPath = mountPath;
		g_loadSprite = loadSprite;
		g_loadTexture = loadTexture;
		g_loadModel = loadModel;
	}
#else
	DLL_API void Initialize(std::wstring_view mountPath)
	{
		g_mountPath = mountPath;
	}
#endif

	DLL_API void CleanUp()
	{
		g_resources.clear();
	}

#ifdef _TOOL
	static bool SaveRecursive(std::ofstream& file, const std::shared_ptr<Property>& prop)
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
			auto binary{ data->GetBinary() };
			auto length{ static_cast<uint32_t>(binary.size()) };
			file.write(reinterpret_cast<const char*>(&length), sizeof(length));
			file.write(reinterpret_cast<const char*>(binary.data()), length);
			break;
		}
		case Property::Type::Texture:
		{
			break;
		}
		case Property::Type::Model:
		{
			auto data{ prop->GetModel() };

			// 총 바이너리 크기
			// 미리 0으로 써둬서 자리를 만들어두고 마지막에 세팅해줌
			uint32_t totalSize{ 0 };
			auto startPos{ file.tellp() };
			file.write(reinterpret_cast<const char*>(&totalSize), sizeof(totalSize));

			// 메쉬 개수
			auto meshCount{ static_cast<uint32_t>(data->meshes.size()) };
			file.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));
			totalSize += sizeof(meshCount);

			// 메쉬 데이터
			for (const auto& mesh : data->meshes)
			{
				// 정점
				auto vertexCount{ static_cast<uint32_t>(mesh.vertices.size()) };
				file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
				totalSize += sizeof(vertexCount);

				file.write(reinterpret_cast<const char*>(&mesh.vertices), sizeof(Model::Mesh::Vertex) * vertexCount);
				totalSize += sizeof(Model::Mesh::Vertex) * vertexCount;

				// 정점 인덱스
				auto indexCount{ static_cast<uint32_t>(mesh.indices.size()) };
				file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
				totalSize += sizeof(indexCount);

				file.write(reinterpret_cast<const char*>(mesh.indices.data()), sizeof(int) * indexCount);
				totalSize += sizeof(int) * indexCount;

				// 재질
				auto materialCount{ static_cast<uint32_t>(mesh.materials.size()) };
				file.write(reinterpret_cast<const char*>(&materialCount), sizeof(materialCount));
				totalSize += sizeof(materialCount);

				file.write(reinterpret_cast<const char*>(mesh.materials.data()), sizeof(Model::Mesh::Material) * materialCount);
				totalSize += sizeof(Model::Mesh::Material) * materialCount;
			}

			auto endPos{ file.tellp() };
			file.seekp(startPos);
			file.write(reinterpret_cast<const char*>(&totalSize), sizeof(totalSize));

			file.seekp(endPos);
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
			if (!SaveRecursive(file, child))
				return false;
		}
		return true;
	}

	DLL_API bool Save(const std::shared_ptr<Property>& prop, std::wstring_view path)
	{
		auto name{ prop->GetName() };
		prop->SetName(Stringtable::DATA_ROOT_NAME);

		std::ofstream file{ path.data(), std::ios::binary };
		if (!file)
		{
			assert(false && "CAN NOT SAVE DATA FILE");
			return false;
		}

		if (!SaveRecursive(file, prop))
			return false;

		prop->SetName(name);
		return true;
	}
#endif

	static std::shared_ptr<Property> LoadRecursive(std::ifstream& file, std::wstring_view subPath)
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
			Int2 data{};
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
#if defined _CLIENT || defined _TOOL
			std::unique_ptr<std::byte[]> binary{ new std::byte[length]{} };
			file.read(reinterpret_cast<char*>(binary.get()), length);

			auto data{ g_loadSprite(std::span{ binary.get(), length }) };
			prop->Set(data);
#else
			file.ignore(length);
#endif
			break;
		}
		case Property::Type::Texture:
		{
			uint32_t length{};
			file.read(reinterpret_cast<char*>(&length), sizeof(length));
#if defined _CLIENT || defined _TOOL
			std::unique_ptr<std::byte[]> binary{ new std::byte[length]{} };
			file.read(reinterpret_cast<char*>(binary.get()), length);

			auto data{ g_loadTexture(std::span{ binary.get(), length }) };
			prop->Set(data);
#else
			file.ignore(length);
#endif
			break;
		}
		case Property::Type::Model:
		{
			uint32_t length{};
			file.read(reinterpret_cast<char*>(&length), sizeof(length));
#if defined _CLIENT || defined _TOOL
			std::unique_ptr<std::byte[]> binary{ new std::byte[length]{} };
			file.read(reinterpret_cast<char*>(binary.get()), length);

			auto data{ g_loadModel(std::span{ binary.get(), length }) };
			prop->Set(data);
#else
			file.ignore(length);
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
			auto child{ LoadRecursive(file, subPath) };
#ifdef _TOOL
			child->SetParent(prop);
#endif
			prop->Add(child);
		}
		return prop;
	}

	static std::shared_ptr<Property> Load(const std::filesystem::path& filePath, std::wstring_view subPath)
	{
		std::ifstream file;
		if (filePath.is_absolute())
			file.open(filePath, std::ios::binary);
		else
			file.open(g_mountPath / filePath, std::ios::binary);
		if (!file)
		{
			assert(false && "CAN NOT LOAD DATA FILE");
			return nullptr;
		}

		auto root{ LoadRecursive(file, subPath) };
		return root;
	}

	DLL_API std::shared_ptr<Property> Get(std::wstring_view path)
	{
		std::wstring filePath{ path };
		std::wstring_view subPath{};

		// 파라미터를 파일 경로와 세부 경로로 나눔
		size_t pos{ path.find(Stringtable::DATA_FILE_EXT) };
		if (pos != std::wstring_view::npos)
		{
			constexpr auto EXT_SIZE{ Stringtable::DATA_FILE_EXT.size() };
			filePath = path.substr(0, pos + EXT_SIZE);
			if (path.size() > pos + EXT_SIZE + 1)
				subPath = path.substr(pos + EXT_SIZE + 1);
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

#ifndef _TOOL
		// 캐싱
		g_resources.emplace(filePath, root);
#endif

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
