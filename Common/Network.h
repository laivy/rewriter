#pragma once

enum class IOOperation
{
	NONE, ACCEPT
};

struct OVERLAPPEDEX : public OVERLAPPED
{
	static constexpr auto BUFFER_SIZE = 512;

	OVERLAPPEDEX() : 
		OVERLAPPED{},
		op{ IOOperation::NONE },
		socket{ INVALID_SOCKET },
		buffer{}
	{
	}

	IOOperation op;
	SOCKET socket;
	char buffer[BUFFER_SIZE];
};