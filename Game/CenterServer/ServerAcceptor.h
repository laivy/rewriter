#pragma once
#include "Server.h"
#include "Common/Socket.h"

class ServerAcceptor : public TSingleton<ServerAcceptor>
{
public:
	ServerAcceptor();
	~ServerAcceptor();

	void Render();

private:
	void LoadConfig();

	void Run(std::stop_token stoken);
	void OnAccept();
	void OnReceive(IServer* server, Packet::Size ioSize);
	void OnDisconnect(IServer* server);

private:
	std::jthread m_thread;

	unsigned short m_port;
	std::vector<std::tuple<IServer::Type, std::wstring, int>> m_serverLists;

	HANDLE m_iocp;

	// Accept 관련
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	std::array<char, 64> m_acceptBuffer;
	OverlappedEx m_overlappedEx;

	std::vector<std::unique_ptr<IServer>> m_servers;
};
