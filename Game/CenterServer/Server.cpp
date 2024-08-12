#include "Stdafx.h"
#include "Server.h"

void IServer::SetSocket(SocketEx&& socket)
{
	m_socket = std::move(socket);
}

SocketEx& IServer::GetSocket()
{
	return m_socket;
}
