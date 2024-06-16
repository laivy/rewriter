#include "Stdafx.h"
#include "User.h"
#include "UserManager.h"

void UserManager::Update()
{
}

void UserManager::Render()
{
}

void UserManager::Register(const std::shared_ptr<User>& user)
{
	// accountid 기준 오름차순 삽입 정렬
	auto it = std::ranges::lower_bound(m_users, user, 
		[](const auto& user1, const auto& user2)
		{
			return user1->GetAccountID() < user2->GetAccountID();
		});
	m_users.insert(it, user);
}

void UserManager::Unregister(const std::shared_ptr<User>& user)
{
}

std::weak_ptr<User> UserManager::GetUser(AccountID id)
{
	auto it = std::ranges::lower_bound(m_users, id,
		[](const auto& user, auto id) -> bool
		{
			return user->GetAccountID() < id;
		});
	if (it == m_users.end())
		return {};
	return *it;
}
