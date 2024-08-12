#pragma once
#include "Common/Socket.h"

class LoginServer :
	public ISocket,
	public TSingleton<LoginServer>
{
public:
	LoginServer();
	~LoginServer() = default;
};