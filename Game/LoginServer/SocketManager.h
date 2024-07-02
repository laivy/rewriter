#pragma once
#include "Common/Socket.h"

class User;

class SocketManager : public TSingleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager();

	void Render();

private:
	void Run(std::stop_token stoken);
	void OnAccept();
	void OnReceive(User* user, Packet::size_type ioSize);
	void OnDisconnect(User* user);

private:
	HANDLE m_hIOCP;
	std::array<std::jthread, 3> m_threads;

	// Accept 관련
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	std::array<char, 64> m_acceptBuffer;
	OVERLAPPEDEX m_overlappedEx;
};