#pragma once
#include "Common/ServerSocket.h"

class CenterServer final :
	public ServerSocket,
	public TSingleton<CenterServer>
{
public:
	CenterServer();
	~CenterServer() = default;

	void OnConnect(bool success) override;
	void OnDisconnect() override;
	void OnComplete(Packet& packet) override;
};
