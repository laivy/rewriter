#pragma once
#include "Common/Socket.h"

class LoginServer final : public ISocket
{
public:
	LoginServer() = default;
	~LoginServer() = default;
};
