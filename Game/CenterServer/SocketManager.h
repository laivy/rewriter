#pragma once
#include "Common/Socket.h"

class ServerSocket;

class SocketManager : public TSingleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager();

	void Render();

private:
	void Run(std::stop_token stoken);
	void Accept(std::stop_token stoken);

	void LoadConfig();
	void Register(ISocket* socket) const;
	void Disconnect(ISocket* server);

private:
	HANDLE m_iocp;
	std::array<std::jthread, 3> m_threads;

	unsigned short m_port;
	//std::vector<std::tuple<IServer::Type, std::wstring, int>> m_serverLists;

	SOCKET m_listenSocket;
	std::vector<std::shared_ptr<ServerSocket>> m_sockets;
};
