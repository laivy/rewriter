#include "Stdafx.h"
#include "User.h"
#include "UserManager.h"

void UserManager::Update(float deltaTime)
{
	std::lock_guard lock{ m_mutex };
	std::for_each(std::execution::par, m_users.begin(), m_users.end(),
		[deltaTime](auto& user)
		{
			user->Update(deltaTime);
		});
}

void UserManager::Render()
{
}

void UserManager::Register(const std::shared_ptr<User>& user)
{
	std::lock_guard lock{ m_mutex };
	m_users.push_back(user);
}

void UserManager::Unregister(User* user)
{
	std::lock_guard lock{ m_mutex };
	std::erase_if(m_users, [user](const auto& u) { return u.get() == user; });
}

std::shared_ptr<User> UserManager::GetUser(std::uint32_t accountID) const
{
	std::lock_guard lock{ m_mutex };
	auto it = std::ranges::find_if(m_users, [accountID](const auto& user) { return user->GetAccountID() == accountID; });
	if (it == m_users.end())
		return nullptr;
	return *it;
}