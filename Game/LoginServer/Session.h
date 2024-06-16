#pragma once

struct Session
{
	static constexpr auto BUFFER_SIZE = 512;

	size_t id{ 0 };
	SOCKET socket{ INVALID_SOCKET };
	OVERLAPPEDEX overlappedEx{};
	std::unique_ptr<char[]> recvBuffer{ new char[BUFFER_SIZE] {} };

	Packet::size_type remainSize{ 0 };
	std::shared_ptr<Packet> packet;
};