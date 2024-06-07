#pragma once
#include "ClientSocket.h"

class ClientAcceptor : public TSingleton<ClientAcceptor>
{
public:
	ClientAcceptor();
	~ClientAcceptor();

	void Render();

private:
	void Init();
	void Run();

	void OnAccept(OVERLAPPEDEX* overlappedEx);
	void OnReceive(size_t socketID);
	void OnDisconnect(size_t socketID);

private:
	static constexpr auto ACCEPT_THREAD_COUNT{ 1 };
	static constexpr auto SOCKET_TIMEOUT_MILLISEC = 500;

	bool m_isActive;

	SOCKET m_acceptSocket;
	HANDLE m_hIOCP;
	OVERLAPPEDEX m_overlappedEx;

	// 클라이언트 소켓
	std::mutex m_socketMutex;
	std::unordered_map<size_t, ClientSocket> m_clientSockets;
	std::atomic<size_t> m_clientSocketID;

	// 워커 쓰레드
	std::array<std::jthread, ACCEPT_THREAD_COUNT> m_threads;

	// IMGUI 로그
	std::mutex m_logMutex;
	std::vector<std::string> m_logs;
};