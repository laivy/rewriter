#pragma once
#include "Common/Socket.h"

class User;

class SocketManager : public TSingleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager() = default;

	void Register(const std::shared_ptr<User>& user, const std::shared_ptr<Socket>& socket);

private:
	void Run(std::stop_token stoken);
	void OnReceive(User* user, unsigned long ioSize);
	void OnDisconnect(User* user);

private:
	HANDLE m_hIOCP;
	std::array<std::jthread, 3> m_threads;
};