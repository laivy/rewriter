#pragma once
#include "Server.h"

class ServerManager : public TSingleton<ServerManager>
{
public:
	ServerManager();
	~ServerManager();

	bool Connect(IServer::Type type, std::wstring_view ip, unsigned short port);
	void Disconnect(IServer::Type type);
	bool IsConnected(IServer::Type type);
	void SendPacket(IServer::Type type, const Packet& packet);

private:
	void Run(std::stop_token stoken);
	void OnReceive(IServer* server, Packet::Size ioSize);
	void OnDisconnect(IServer* server);

private:
	std::jthread m_thread;

	HANDLE m_iocp;
	std::vector<std::unique_ptr<IServer>> m_servers;
};