#include "stdafx.h"
#include "NytLoader.h"

NytProperty& NytLoader::Load(const std::string& filePath)
{
	// 이미 로딩된 데이터인지 확인
	if (m_data.contains(filePath))
		return m_data[filePath];

	std::ifstream ifstream{ StringTable::DATA_FOLDER_PATH + filePath, std::ifstream::binary };
	assert(ifstream);

	// 루트 프로퍼티 개수
	int nodeCount{};
	ifstream.read(reinterpret_cast<char*>(&nodeCount), sizeof(int));

	// 순회하며 모든 프로퍼티 로딩
	NytProperty root{};
	for (int i = 0; i < nodeCount; ++i)
	{
		Load(ifstream, root);
	}

	// 저장 후 반환
	m_data[filePath] = std::move(root);
	return m_data[filePath];
}

void NytLoader::Load(std::ifstream& fs, NytProperty& root)
{
	auto ReadString = [&]() -> std::string
	{
		char length{};
		char buffer[50]{};
		fs.read(&length, sizeof(char));
		fs.read(buffer, length);
		return std::string{ buffer };
	};

	NytDataType type{};
	fs.read(reinterpret_cast<char*>(&type), sizeof(byte));

	std::string name{ ReadString() };
	std::any data{};

	switch (type)
	{
	case NytDataType::GROUP:
		break;
	case NytDataType::INT:
	{
		int buffer{};
		fs.read(reinterpret_cast<char*>(&buffer), sizeof(int));
		data = buffer;
		break;
	}
	case NytDataType::FLOAT:
	{
		float buffer{};
		fs.read(reinterpret_cast<char*>(&buffer), sizeof(float));
		data = buffer;
		break;
	}
	case NytDataType::STRING:
		data = ReadString();
		break;
	case NytDataType::IMAGE:
	{
		int len{};
		std::unique_ptr<char> buffer{ new char[10 * 1024] };
		fs.read(reinterpret_cast<char*>(&len), sizeof(int));
		fs.read(buffer.get(), len);
		data = std::string{ buffer.get() };
		break;
	}
	default:
		assert(false);
	}

	int childNodeCount{};
	fs.read(reinterpret_cast<char*>(&childNodeCount), sizeof(int));

	root.m_childNames.reserve(childNodeCount);
	root.m_childNames.push_back(name);
	root.m_childProps[name] = NytProperty{ type,data };

	for (int i = 0; i < childNodeCount; ++i)
	{
		Load(fs, root.m_childProps[name]);
	}
}

NytProperty::NytProperty() : m_type{ NytDataType::GROUP }, m_data{}
{

}

NytProperty::NytProperty(NytDataType type, const std::any& data) : m_type{ type }, m_data{ data }
{

}