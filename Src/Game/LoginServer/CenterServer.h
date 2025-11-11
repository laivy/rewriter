#pragma once
#include "Common/ServerSocket.h"

class CenterServer final :
	public ServerSocket,
	public Singleton<CenterServer>
{
public:
	CenterServer();
	~CenterServer() = default;

	void OnConnect(bool success) override;
	void OnDisconnect() override;
	void OnComplete(Packet& packet) override;

private:
	bool Connect();

	void OnLoginRequest(Packet& packet);
	void OnRegisterAccountRequest(Packet& packet);
};
