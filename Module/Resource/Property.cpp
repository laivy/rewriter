#include "Stdafx.h"
#include "Include/Image.h"
#include "Include/Property.h"
#include "Include/ResourceManager.h"
#include "External/DirectX/WICTextureLoader12.h"

namespace Resource
{
	Property::Iterator::Iterator(const Property* const p, size_t index) :
		m_property{ p },
		m_childIndex{ index }
	{
	}

	Property::Iterator& Property::Iterator::operator++()
	{
		++m_childIndex;
		return *this;
	}

	Property::Iterator& Property::Iterator::operator--()
	{
		--m_childIndex;
		return *this;
	}

	bool Property::Iterator::operator!=(const Iterator& it) const
	{
		if (m_property != it.m_property)
			return true;
		if (m_childIndex != it.m_childIndex)
			return true;
		return false;
	}

	std::pair<std::string, std::shared_ptr<Property>> Property::Iterator::operator*() const
	{
		return std::make_pair(
			m_property->m_children[m_childIndex]->m_name,
			m_property->m_children[m_childIndex]
		);
	}

	Property::Property() :
		m_type{ Property::Type::GROUP },
		m_name{ "" },
		m_int{ 0 }
	{
	}

	Property::~Property()
	{
	}

	Property::Iterator Property::begin() const
	{
		return Iterator{ this, 0 };
	}

	Property::Iterator Property::end() const
	{
		return Iterator{ this, m_children.size() - 1 };
	}

	Property::Type Property::GetType() const
	{
		return m_type;
	}

	std::shared_ptr<Property> Property::Get(const std::string& path) const
	{
		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			std::string childName{ path.substr(0, pos) };
			std::string remain{ path.substr(pos + 1) };
			auto it{ std::ranges::find_if(m_children, [&](const auto& p) { return p->m_name == childName; }) };
			if (it == m_children.end())
				return nullptr;
			return (*it)->Get(remain);
		}

		auto it{ std::ranges::find_if(m_children, [&](const auto& p) { return p->m_name == path; }) };
		if (it == m_children.end())
			return nullptr;
		return *it;
	}

	int Property::GetInt() const
	{
		assert(m_type == Type::INT);
		return m_int;
	}

	INT2 Property::GetInt2() const
	{
		assert(m_type == Type::INT2);
		return m_int2;
	}

	float Property::GetFloat() const
	{
		assert(m_type == Type::FLOAT);
		return m_float;
	}

	std::string Property::GetString() const
	{
		assert(m_type == Type::STRING);
		return m_string;
	}

	std::shared_ptr<Image> Property::GetImage() const
	{
		assert(m_type == Type::IMAGE);
		return m_image;
	}

	ID2D1Bitmap* Property::GetD2DImage() const
	{
		assert(m_type == Type::IMAGE);
		//return m_image.GetD2DImage();
		return nullptr;
	}

	ComPtr<ID3D12Resource> Property::GetD3DImage() const
	{
		//assert(m_type == Type::IMAGE || !m_binary.empty());
		//return m_d3dResource;
		return nullptr;
	}

	void Property::SetD3DImage(const ComPtr<ID3D12Resource>& image)
	{
		assert(m_type == Type::IMAGE);
		//m_binary.clear();
		//m_d3dResource = image;
	}

	void Property::Load(std::ifstream& file, std::string& name)
	{
		// 이름
		char length{};
		file.read(reinterpret_cast<char*>(&length), sizeof(length));

		std::array<char, 128> buffer{};
		file.read(buffer.data(), length);

		m_name.assign(buffer.data(), length);

		// 타입
		file.read(reinterpret_cast<char*>(&m_type), sizeof(m_type));

		// 데이터
		switch (m_type)
		{
		case Type::GROUP:
			break;
		case Type::INT:
			file.read(reinterpret_cast<char*>(&m_int), sizeof(m_int));
			break;
		case Type::INT2:
			file.read(reinterpret_cast<char*>(&m_int2), sizeof(m_int2));
			break;
		case Type::FLOAT:
			file.read(reinterpret_cast<char*>(&m_float), sizeof(m_float));
			break;
		case Type::STRING:
		{
			length = 0;
			file.read(reinterpret_cast<char*>(&length), sizeof(length));
			buffer.fill(0);
			file.read(buffer.data(), length);
			m_string = std::string{ buffer.data(), static_cast<size_t>(length) };
			break;
		}
		case Type::IMAGE:
		{
			int len{};
			file.read(reinterpret_cast<char*>(&len), sizeof(len));

			std::unique_ptr<BYTE[]> binary{ new BYTE[len] };
			file.read(reinterpret_cast<char*>(binary.get()), len);

			m_image = std::make_unique<Image>(binary.release(), static_cast<DWORD>(len));
			break;
		}
		default:
			assert(false);
			break;
		}

		// 자식 노드
		int nodeCount{};
		file.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

		if (nodeCount > 0)
		{
			m_children.reserve(nodeCount);
			for (int i = 0; i < nodeCount; ++i)
			{
				std::string temp{ name };
				if (IsSkip(file, temp))
				{
					Skip(file);
					continue;
				}
				m_children.emplace_back(new Property);
				m_children.back()->Load(file, temp);
			}
		}
	}

	void Resource::Property::Flush()
	{
		std::erase_if(m_children, [](const auto& c) { return c.use_count() <= 1; });
	}
}