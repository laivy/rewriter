#pragma once
#include "Shared/Socket.h"

class ServerSocket final : public ISocket
{
public:
	ServerSocket(SOCKET socket);
	~ServerSocket() = default;

	void OnComplete(Packet& packet) override;

private:
	// 로그인 서버 관련
	void OnLoginServerPacket(Packet& packet);
	void OnLoginRequest(Packet& packet);
	void OnRegisterAccountRequest(Packet& packet);
};
