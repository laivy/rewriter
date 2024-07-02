#pragma once

struct Socket;

struct Account
{
	std::uint32_t id{ 0 };
	std::string name;
};

class User
{
public:
	User(const std::shared_ptr<Socket>& socket);
	~User() = default;

	void OnPacket(Packet& packet);

	void Update(float deltaTime);

	const std::shared_ptr<Socket>& GetSocket() const;
	std::uint32_t GetAccountID() const;

private:
	std::shared_ptr<Socket> m_socket;
	Account m_account;
};