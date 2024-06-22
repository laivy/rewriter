#pragma once

class Acceptor : 
	public std::jthread,
	public TSingleton<Acceptor>
{
public:
	Acceptor();
	~Acceptor();

	void Render();

private:
	void Run();
	void OnAccept();

private:
	static constexpr auto SOCKET_TIMEOUT_MILLISEC = 1000;

	bool m_isActive;

	HANDLE m_hIOCP;
	SOCKET m_listenSocket;
	SOCKET m_clientSocket;
	std::array<char, 64> m_buffer;
	OVERLAPPED m_overlapped;

	std::mutex m_logMutex;
	std::vector<std::string> m_logs;
};