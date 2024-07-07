#pragma once
#include "Server.h"

class LoginServer final : public IServer
{
public:
	LoginServer();
	~LoginServer() = default;
};