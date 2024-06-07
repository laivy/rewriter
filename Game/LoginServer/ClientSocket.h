#pragma once

struct ClientSocket
{
	size_t socketID{ 0 };
	SOCKET socket{ INVALID_SOCKET };
	OVERLAPPEDEX overlappedEx{};
};