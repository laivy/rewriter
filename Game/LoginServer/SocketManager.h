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
	void OnAccept();

	void Disconnect(ClientSocket* socket);

private:
	HANDLE m_iocp;
	std::array<std::jthread, 3> m_threads;

	// Accept 관련
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	std::array<char, 64> m_acceptBuffer;
	OVERLAPPEDEX m_overlappedEx;

	std::vector<std::shared_ptr<ClientSocket>> m_sockets;
};