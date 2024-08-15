#pragma once
#include "Common/Socket.h"

class ServerSocket : public ISocket
{
public:
	ServerSocket(SOCKET socket);
	~ServerSocket() = default;
};
