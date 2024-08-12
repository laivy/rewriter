#pragma once
#include "Common/Socket.h"

class User;

class ClientSocket : public ISocket
{
public:
	ClientSocket(SOCKET socket);
	~ClientSocket();

private:
	void OnPacket(Packet& packet);
	void OnRegisterRequest(Packet& packet);
	void OnLoginRequest(Packet& packet);

private:
	std::shared_ptr<User> m_user;
};