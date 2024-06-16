#pragma once
#include "Session.h"

class AcceptThread : 
	public std::jthread,
	public TSingleton<AcceptThread>
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
	AcceptThread();
	~AcceptThread();

	void Render();

private:
	void Run();

	void OnAccept(OVERLAPPEDEX* overlappedEx);
	void OnReceive(size_t sessionID, Packet::size_type ioSize);
	void OnDisconnect(size_t sessionID);
	void OnPacket(size_t sessionID, const std::shared_ptr<Packet>& packet);

private:
	static constexpr auto SOCKET_TIMEOUT_MILLISEC = 1000;

	bool m_isActive;
	AcceptContext m_context;

	// 클라이언트 세선
	std::atomic<size_t> m_sessionID;
	std::unordered_map<size_t, Session> m_sessions;

	// IMGUI 로그
	std::mutex m_logMutex;
	std::vector<std::string> m_logs;
};