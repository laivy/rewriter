#pragma once

struct Session
{
	size_t id{ 0 };
	SOCKET socket{ INVALID_SOCKET };
	OVERLAPPEDEX recvOverlappedEx{};

	unsigned int remainSize{ 0 };
	std::unique_ptr<Packet> packet;
};