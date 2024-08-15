#pragma once
#include "ClientSocket.h"

class SocketManager : public TSingleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager();

	void Render();

private:
	void Run(std::stop_token stoken);
	void Accept(std::stop_token stoken);

	void Register(ISocket* socket) const;
	void Disconnect(ClientSocket* socket);

private:
	HANDLE m_iocp;
	std::array<std::jthread, 3> m_threads;

	SOCKET m_listenSocket;
	std::vector<std::shared_ptr<ClientSocket>> m_sockets;
};
