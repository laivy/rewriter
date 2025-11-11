#pragma once
#include "Common/Socket.h"

class SocketManager : public Singleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager();

	void Render();

	void Register(std::unique_ptr<ISocket> socket);
	void Disconnect(ISocket* socket);

private:
	void Run(std::stop_token stoken);

	void OnAccept();
	void Accept();

	bool IsInWhitelist(SOCKET socket) const;

#ifdef _IMGUI
	void Logging(const std::string& log);
#endif

private:
	HANDLE m_iocp;
	SOCKET m_listenSocket;

	// 워커쓰레드
	std::array<std::jthread, 3> m_threads;

	// Accept 관련 락, 변수
	std::recursive_mutex m_acceptMutex;
	SOCKET m_acceptSocket;
	std::array<char, 64> m_acceptBuffer;
	ISocket::OverlappedEx m_acceptOverlappedEx;
	std::list<std::unique_ptr<ISocket>> m_sockets;

	// 서버 설정
	std::shared_ptr<Resource::Property> m_config;

#ifdef _IMGUI
	std::vector<std::string> m_logs;
#endif
};
