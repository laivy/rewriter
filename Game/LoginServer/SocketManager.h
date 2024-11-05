#pragma once

class SocketManager : public TSingleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager();

	void Render();

private:
	void Run(std::stop_token stoken);

	void OnConnect();
	void Accept();

	void Register(ISocket* socket) const;
	void Disconnect(ISocket* socket);

#ifdef _IMGUI
	void Logging(const std::string& log);
#endif

private:
	HANDLE m_iocp;
	SOCKET m_listenSocket;

	// Accept 관련 락, 변수
	std::recursive_mutex m_acceptMutex;
	SOCKET m_acceptSocket;
	std::array<char, 64> m_acceptBuffer;
	ISocket::OverlappedEx m_acceptOverlappedEx;
	std::vector<std::shared_ptr<ISocket>> m_sockets;

	std::array<std::jthread, 3> m_threads;

#ifdef _IMGUI
	std::vector<std::string> m_logs;
#endif
};
