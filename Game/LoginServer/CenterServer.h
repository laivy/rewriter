#pragma once
#include "Common/ServerSocket.h"

class CenterServer final :
	public ServerSocket,
	public TSingleton<CenterServer>
{
public:
	CenterServer();
	~CenterServer() = default;

	virtual void OnConnect() override;
	virtual void OnDisconnect() override;
	virtual void OnComplete(Packet& packet) override;

private:
	std::shared_ptr<Resource::Property> m_config;
};
