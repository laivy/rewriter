#include "Pch.h"
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

	RecursiveIterator::RecursiveIterator(ID id)
	{
		m_stacks.emplace_back(id);
	}

	RecursiveIterator::RecursiveIterator(const std::wstring& path)
	{
		m_stacks.emplace_back(path);
	}

	RecursiveIterator::value_type RecursiveIterator::operator*() const
	{
		return *m_stacks.back();
	}

	RecursiveIterator& RecursiveIterator::operator++()
	{
		auto& last{ m_stacks.back() };
		const auto& [_, id] { *last };
		++last;

		// 자식이 있으면 자식을 순회
		if (auto manager{ Manager::GetInstance() }; manager && manager->GetChildCount(id) > 0)
		{
			m_stacks.emplace_back(id);
			return *this;
		}

		while (last == last.end())
		{
			m_stacks.pop_back();
			if (m_stacks.empty())
				break;
			last = m_stacks.back();
		}
		return *this;
	}

	bool RecursiveIterator::operator==(const RecursiveIterator& other) const
	{
		return m_stacks == other.m_stacks;
	}

	RecursiveIterator RecursiveIterator::begin() const
	{
		return *this;
	}

	RecursiveIterator RecursiveIterator::end() const
	{
		return RecursiveIterator{};
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

	void Move(ID targetID, ID parentID, std::optional<std::size_t> index)
	{
		if (auto manager{ Manager::GetInstance() })
			manager->Move(targetID, parentID, index);
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

	std::optional<std::wstring> GetName(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->GetName(id);
		return std::nullopt;
	}

	std::optional<std::wstring> GetName(ID id, const std::wstring &path)
	{
		auto manager{ Manager::GetInstance() };
		if (!manager)
			return std::nullopt;

		const ID targetID{ manager->Get(id, path) };
		if (targetID == InvalidID)
			return std::nullopt;

		return manager->GetName(targetID);
	}

	std::optional<std::int32_t> GetInt(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get<std::int32_t>(id);
		return std::nullopt;
	}

	std::optional<std::int32_t> GetInt(const ID id, const std::wstring& path)
	{
		auto manager{ Manager::GetInstance() };
		if (!manager)
			return std::nullopt;

		const ID targetID{ manager->Get(id, path) };
		if (targetID == InvalidID)
			return std::nullopt;

		return manager->Get<std::int32_t>(targetID);
	}

	std::optional<float> GetFloat(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get<float>(id);
		return std::nullopt;
	}

	std::optional<float> GetFloat(const ID id, const std::wstring& path)
	{
		auto manager{ Manager::GetInstance() };
		if (!manager)
			return std::nullopt;

		const ID targetID{ manager->Get(id, path) };
		if (targetID == InvalidID)
			return std::nullopt;

		return manager->Get<float>(targetID);
	}

	std::optional<std::wstring> GetString(const ID id)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->Get<std::wstring>(id);
		return std::nullopt;
	}

	std::optional<std::wstring> GetString(const ID id, const std::wstring& path)
	{
		auto manager{ Manager::GetInstance() };
		if (!manager)
			return std::nullopt;

		const ID targetID{ manager->Get(id, path) };
		if (targetID == InvalidID)
			return std::nullopt;

		return manager->Get<std::wstring>(targetID);
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

	ID LoadFromFile(const std::filesystem::path& filePath, const std::wstring& subPath)
	{
		if (auto manager{ Manager::GetInstance() })
			return manager->LoadFromFile(filePath, subPath);
		return InvalidID;
	}
}
