#pragma once
#include "User.h"

class UserManager : public TSingleton<UserManager>
{
public:
	UserManager() = default;
	~UserManager() = default;

	void Update();
	void Render();

	void Register(const std::shared_ptr<User>& user);
	void Unregister(const std::shared_ptr<User>& user);

	std::weak_ptr<User> GetUser(AccountID id);

private:
	std::vector<std::shared_ptr<User>> m_users;
};