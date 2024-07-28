#pragma once
#include "Common/Socket.h"

class IServer abstract
{
public:
	enum class Type
	{
		LOGIN,
		GAME
	};

public:
	IServer() = default;
	virtual ~IServer() = default;

	void SetSocket(SocketEx&& socket);

	SocketEx& GetSocket();

protected:
	SocketEx m_socket;
};