#pragma once
#include "Common/Socket.h"

class ServerSocket final : public ISocket
{
public:
	ServerSocket(SOCKET socket);
	~ServerSocket() = default;

	void OnComplete(Packet& packet) override;

private:
	void OnLoginPacket(Packet& packet);
};
