#pragma once
#include "Common/Socket.h"

class CenterServer final :
	public ISocket,
	public TSingleton<CenterServer>
{
public:
	CenterServer();
	~CenterServer() = default;

	virtual void OnDisconnect() override;
	virtual void OnComplete(Packet& packet) override;

private:
	std::shared_ptr<Resource::Property> m_config;
};
