#include <array>
#include <memory>
#include "WinSock2.h"

class Packet;

enum class IOOP
{
	ACCEPT,
	RECEIVE,
};

struct OVERLAPPEDEX : OVERLAPPED
{
	IOOP op{ IOOP::ACCEPT };
};

struct Socket
{
	SOCKET socket{ INVALID_SOCKET };
	OVERLAPPEDEX overlappedEx{};
	std::array<char, 512> buffer{};
	std::uint32_t remainSize{};
	std::unique_ptr<Packet> packet;
};