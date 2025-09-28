#include "Stdafx.h"
#include "Manager.h"
#include "Property.h"

namespace Resource
{
	Iterator::Iterator() :
		m_parentID{ InvalidID },
		m_current{ 0 },
		m_end{ 0 }
	{
	}

	Iterator::Iterator(ID id) :
		m_parentID{ id },
		m_current{ 0 },
		m_end{ 0 }
	{
		if (auto manager{ Manager::GetInstance() })
			m_end = manager->GetChildCount(m_parentID);
	}

	Iterator::Iterator(const std::wstring& path) :
		Iterator{}
	{
		if (auto manager{ Manager::GetInstance() })
			m_parentID = manager->Get(path);
	}

	Iterator::value_type Iterator::operator*() const
	{
		auto manager{ Manager::GetInstance() };
		if (!manager)
			return { L"", InvalidID };

		const auto id{ manager->GetChild(m_parentID, m_current) };
		if (id == InvalidID)
			return { L"", InvalidID };

		const auto name{ manager->GetName(id) };
		return { name, id };
	}

	Iterator& Iterator::operator++()
	{
		++m_current;
		return *this;
	}

	Iterator Iterator::operator++(int)
	{
		auto it{ *this };
		++m_current;
		return it;
	}

	Iterator& Iterator::operator--()
	{
		--m_current;
		return *this;
	}

	Iterator Iterator::operator--(int)
	{
		auto it{ *this };
		--m_current;
		return it;
	}

	Iterator& Iterator::operator+=(const difference_type offset)
	{
		m_current += offset;
		return *this;
	}

	Iterator Iterator::operator+(const difference_type offset) const
	{
		auto it{ *this };
		it.m_current += offset;
		return it;
	}

	Iterator& Iterator::operator-=(const difference_type offset)
	{
		m_current -= offset;
		return *this;
	}

	Iterator Iterator::operator-(const difference_type offset) const
	{
		auto it{ *this };
		it.m_current -= offset;
		return it;
	}

	Iterator::difference_type Iterator::operator-(const Iterator& other) const
	{
		if (m_parentID != other.m_parentID)
		{
			assert(false && "different parentID");
			return 0;
		}
		return static_cast<difference_type>(m_current) - static_cast<difference_type>(other.m_current);
	}

	Iterator::value_type Iterator::operator[](const difference_type offset) const
	{
		const auto index{ static_cast<std::size_t>(static_cast<difference_type>(m_current) + offset) };
		if (index >= m_end)
		{
			assert(false && "out of range");
			return { L"", InvalidID };
		}

		auto manager{ Manager::GetInstance() };
		if (!manager)
			return { L"", InvalidID };

		const auto id{ manager->GetChild(m_parentID, index) };
		if (id == InvalidID)
			return { L"", InvalidID };

		auto name{ manager->GetName(id) };
		return { name, id };
	}

	bool Iterator::operator==(const Iterator& other) const
	{
		if (m_parentID != other.m_parentID)
			return false;
		if (m_current != other.m_current)
			return false;
		if (m_end != other.m_end)
			return false;
		return true;
	}

	Iterator Iterator::begin() const
	{
		return *this;
	}

	Iterator Iterator::end() const
	{
		auto it{ *this };
		it.m_current = it.m_end;
		return it;
	}

	ID New(const std::wstring& name)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->New(name);
		return InvalidID;
	}

	ID New(ID parentID, const std::wstring& name)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->New(parentID, name);
		return InvalidID;
	}

	void Delete(ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			manager->Delete(id);
	}

	ID Get(const std::wstring& path)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get(path);
		return InvalidID;
	}

	ID Get(ID id, const std::wstring& path)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get(id, path);
		return InvalidID;
	}

	ID GetParent(ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->GetParent(id);
		return InvalidID;
	}

	void SetName(ID id, const std::wstring& name)
	{
		if (auto manager{ Manager::GetInstance() })
			manager->SetName(id, name);
	}

	void Set(ID id, std::int32_t value)
	{
		if (auto manager{ Manager::GetInstance() })
			manager->Set(id, value);
	}

	void Set(ID id, float value)
	{
		if (auto manager{ Manager::GetInstance() })
			manager->Set(id, value);
	}

	void Set(ID id, const std::wstring& value)
	{
		if (auto manager{ Manager::GetInstance() })
			manager->Set(id, value);
	}

	//void Set(const ID id, const Sprite& value)
	//{
	//	Manager::GetInstance().Set(id, value);
	//}

	std::wstring GetName(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->GetName(id);
		return L"";
	}

	std::optional<std::int32_t> GetInt(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get<std::int32_t>(id);
		return std::nullopt;
	}

	std::optional<float> GetFloat(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get<float>(id);
		return std::nullopt;
	}

	std::optional<std::wstring> GetString(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get<std::wstring>(id);
		return std::nullopt;
	}

	//std::optional<Sprite> GetSprite(const ID id)
	//{
	//	return Manager::GetInstance().Get<Sprite>(id);
	//}

	//void Unload(const ID id)
	//{
	//	Manager::GetInstance().Delete(id);
	//}

	bool SaveToFile(const ID id, const std::filesystem::path& path)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->SaveToFile(id, path);
		return false;
	}
}
