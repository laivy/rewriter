#pragma once

class User;

class UserManager : public Singleton<UserManager>
{
public:
	UserManager() = default;
	~UserManager() = default;

	void Update(float deltaTime);
	void Render();

	void Register(const std::shared_ptr<User>& user);
	void Unregister(User* user);

	std::shared_ptr<User> GetUser(std::uint32_t accountID) const;

private:
	mutable std::mutex m_mutex;
	std::vector<std::shared_ptr<User>> m_users;
};
