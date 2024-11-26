#pragma once
#include "Common/Socket.h"

class User;

class ClientSocket : public ISocket
{
public:
	ClientSocket(SOCKET socket);
	~ClientSocket();

	virtual void OnComplete(Packet& packet) override final;

private:
	void OnAccountRegisterRequest(Packet& packet);
	void OnLoginRequest(Packet& packet);

private:
	std::shared_ptr<User> m_user;
};
