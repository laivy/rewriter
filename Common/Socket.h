#pragma once
#include <WinSock2.h>
#include <array>
#include <memory>

class Packet;

struct OVERLAPPEDEX : OVERLAPPED
{
#ifndef _CLIENT
	enum class IOOP
	{
		ACCEPT,
		RECEIVE,
	};

	IOOP op{ IOOP::ACCEPT };
#endif
};

struct SocketEx
{
	SOCKET socket{ INVALID_SOCKET };
	OVERLAPPEDEX overlappedEx{};
	std::array<char, 512> buffer{};

	std::unique_ptr<Packet> packet;
	Packet::Size remainSize{};
};