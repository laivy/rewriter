#pragma once
#include "Common/Socket.h"

class LoginServer final :
	public ISocket,
	public TSingleton<LoginServer>
{
public:
	LoginServer();
	~LoginServer() = default;

	void OnConnect(bool success) override;
	void OnComplete(Packet& packet) override;

public:
	Delegate<Packet&> OnPacket;
};
