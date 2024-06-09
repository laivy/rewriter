#pragma once
#include "ClientSocket.h"

class ClientAcceptor : public TSingleton<ClientAcceptor>
{
private:
	struct AcceptContext
	{
		HANDLE hIOCP{ INVALID_HANDLE_VALUE };
		SOCKET listenSocket{ INVALID_SOCKET };
		SOCKET clientSocket{ INVALID_SOCKET };
		OVERLAPPEDEX overlappedEx{};
	};

public:
	ClientAcceptor();
	~ClientAcceptor();

	void Render();

private:
	void Init();
	void Run();

	void OnAccept(OVERLAPPEDEX* overlappedEx);
	void OnReceive(size_t socketID, unsigned long ioSize);
	void OnDisconnect(size_t socketID);

private:
	static constexpr auto ACCEPT_THREAD_COUNT{ 1 };
	static constexpr auto SOCKET_TIMEOUT_MILLISEC = 500;

	bool m_isActive;

	AcceptContext m_acceptContext;

	// 클라이언트 소켓
	std::mutex m_socketMutex;
	std::atomic<size_t> m_sessionId;
	std::unordered_map<size_t, Session> m_sessions;

	// 워커 쓰레드
	std::array<std::jthread, ACCEPT_THREAD_COUNT> m_threads;

	// IMGUI 로그
	std::mutex m_logMutex;
	std::vector<std::string> m_logs;
};