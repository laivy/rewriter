#pragma once

enum class IOOperation
{
	ACCEPT, RECV
};

struct OVERLAPPEDEX : public OVERLAPPED
{
	OVERLAPPEDEX() :
		OVERLAPPED{},
		op{ IOOperation::ACCEPT },
		acceptBuffer{}
	{
	}

	IOOperation op;
	std::array<char, 64> acceptBuffer;
};