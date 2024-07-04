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

	void SetSocket(Socket&& socket);

	Socket& GetSocket();

protected:
	Socket m_socket;
};