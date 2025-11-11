#pragma once
#include "Common/Socket.h"

class SocketManager : public Singleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager();

	void Render();

	bool Register(ISocket* socket) const;
	void Disconnect(ISocket* socket);

	void Log(const std::string& log);

	std::shared_ptr<ISocket> GetSocket(ISocket::ID id) const;

private:
	void Run(std::stop_token stoken);

	void OnConnect(ISocket* socket, bool success);
	void OnAccept();
	void Accept();

private:
	HANDLE m_iocp;
	SOCKET m_listenSocket;

	// 워커쓰레드
	std::array<std::jthread, 3> m_threads;

	// Accept 관련 변수
	std::recursive_mutex m_acceptMutex;
	SOCKET m_acceptSocket;
	std::array<char, 64> m_acceptBuffer;
	ISocket::OverlappedEx m_acceptOverlappedEx;
	std::list<std::shared_ptr<ISocket>> m_sockets;

#ifdef _IMGUI
	std::vector<std::string> m_logs;
#endif
};
