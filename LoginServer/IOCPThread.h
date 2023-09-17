#pragma once

class User;

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

private:
	bool m_isActive;
	bool m_isAcceptable;

	HANDLE m_hIOCP;
	SOCKET m_listenSocket;
	OVERLAPPEDEX m_overlappedEx;
	std::vector<std::thread> m_workerThreads;

	// IMGUI 로그
	std::mutex m_logsMutex;
	std::vector<std::wstring> m_logs;
};