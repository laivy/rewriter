#include <array>
#include <memory>
#include "WinSock2.h"

class Packet;

struct Socket
{
	SOCKET socket{ INVALID_SOCKET };
	OVERLAPPED overlapped{};
	std::array<char, 512> buffer{};
	std::uint32_t remainSize{};
	std::unique_ptr<Packet> packet;
};