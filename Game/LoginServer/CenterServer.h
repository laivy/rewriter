#pragma once
#include "Common/Socket.h"

class CenterServer :
	public ISocket,
	public TSingleton<CenterServer>
{
public:
	CenterServer();
	~CenterServer() = default;
};
