#pragma once
#include "LocalSocket.h"

class LoginServer :
	public LocalSocket,
	public TSingleton<LoginServer>
{
public:
	LoginServer();
	~LoginServer() = default;
};