#pragma once
#include "Common/Socket.h"

struct Server
{
	enum class Type
	{
		LOGIN,
		GAME,
		CHAT
	};

	std::string ip;
	unsigned short port{ 0 };
	Socket socket{};
};

class ServerManager : public TSingleton<ServerManager>
{
public:
	ServerManager();
	~ServerManager();

	bool Connect(Server::Type type, std::string_view ip, unsigned short port);
	void Disconnect(Server::Type type);
	bool IsConnected(Server::Type type);
	void SendPacket(Server::Type type, const Packet& packet);

private:
	void Run(std::stop_token stoken);
	void OnReceive(Server::Type type, Packet::size_type ioSize);
	void OnDisconnect(Server::Type type);

private:
	std::jthread m_thread;

	HANDLE m_hIOCP;
	std::map<Server::Type, Server> m_servers;
};