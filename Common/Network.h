#pragma once

enum class IOOperation
{
	NONE, ACCEPT, RECV
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
		wsaBuf.len = BUFFER_SIZE;
		wsaBuf.buf = new char[BUFFER_SIZE] {};
	}

	IOOperation op;
	SOCKET socket;
	char buffer[BUFFER_SIZE];
	WSABUF wsaBuf;
};