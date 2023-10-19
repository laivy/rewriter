#pragma once
#include "Server.h"

class LoginServer :	
	public IServer,
	public TSingleton<LoginServer>
{
public:
	LoginServer();
	~LoginServer();

	virtual bool Connect() final;
	virtual void Send(Packet& packet) final;

	virtual bool IsConnected() const final;

private:
	void Run();

private:
	std::jthread m_thread;
	bool m_isConnected;
	SOCKET m_socket;
};