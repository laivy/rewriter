#include <WinSock2.h>
#include <array>
#include <memory>

class Packet;

struct OVERLAPPEDEX : OVERLAPPED
{
	enum class IOOP
	{
		ACCEPT,
		RECEIVE,
	};

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