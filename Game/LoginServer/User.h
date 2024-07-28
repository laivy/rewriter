#pragma once

class ClientSocket;

class User
{
public:
	User(ClientSocket* socket);
	~User() = default;

	void OnPacket(Packet& packet);

	void Update(float deltaTime);

	int GetAccountID() const;

private:
	ClientSocket* const m_socket;
};