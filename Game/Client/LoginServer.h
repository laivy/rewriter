#pragma once
#include "Common/Socket.h"

class LoginServer :
	public ISocket,
	public TSingleton<LoginServer>
{
public:
	LoginServer();
	~LoginServer() = default;

	virtual void OnPacket(Packet& packet) override final;
	virtual void OnDisconnect() override final;
};
