#pragma once
#include "Shared/Socket.h"

class User;

class ClientSocket : public ISocket
{
public:
	ClientSocket(SOCKET socket);
	~ClientSocket();

	virtual void OnComplete(Packet& packet) override final;

private:
	void OnLoginRequest(Packet& packet);
	void OnAccountRegister(Packet& packet);

private:
	std::shared_ptr<User> m_user;
};
