#pragma once

class ISocket;

class SocketManager : public Singleton<SocketManager>
{
public:
	SocketManager();
	~SocketManager();

	void Register(ISocket* socket) const;

private:
	void Run(std::stop_token stoken);

private:
	HANDLE m_iocp;
	std::jthread m_thread;
};
