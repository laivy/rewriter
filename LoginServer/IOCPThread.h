#pragma once
#include "ClientSocket.h"

class IOCPThread :
	public std::thread,
	public TSingleton<IOCPThread>
{
public:
	IOCPThread();
	~IOCPThread();

	void Render();

private:
	void Run();
	void Work();
	void OnAccept(OVERLAPPEDEX* overlappedEx);
	void OnRecv(int socketID);
	void OnDisconnect(int socketID);

private:
	static constexpr auto SOCKET_TIMEOUT_MILLISEC = 10 * 1000;

	bool m_isActive;
	bool m_isAcceptable;

	HANDLE m_hIOCP;
	SOCKET m_listenSocket;
	OVERLAPPEDEX m_overlappedEx;
	std::atomic<int> m_lastSocketID;
	std::vector<std::thread> m_workerThreads;

	std::mutex m_socketMutex;
	std::unordered_map<int, ClientSocket> m_clientSockets;

	// IMGUI 로그
	std::mutex m_logMutex;
	std::vector<std::wstring> m_logs;
};