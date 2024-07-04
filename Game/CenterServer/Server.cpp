#include "Stdafx.h"
#include "Server.h"

void IServer::SetSocket(Socket&& socket)
{
	m_socket = std::move(socket);
}

Socket& IServer::GetSocket()
{
	return m_socket;
}
