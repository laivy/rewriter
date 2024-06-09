#pragma once

enum class IOOperation
{
	ACCEPT, RECV
};

struct OVERLAPPEDEX : public OVERLAPPED
{
	static constexpr auto BUFFER_SIZE = 512;

	OVERLAPPEDEX() :
		OVERLAPPED{},
		op{ IOOperation::ACCEPT },
		acceptBuffer{},
		recvBuffer{ new char[BUFFER_SIZE] {} }
	{
	}

	IOOperation op;
	std::array<char, 64> acceptBuffer;
	std::shared_ptr<char[]> recvBuffer;
};