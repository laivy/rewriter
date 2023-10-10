#include "Stdafx.h"
#include "ClientSocket.h"

ClientSocket::ClientSocket(int socketID, SOCKET socket) :
	m_socketID{ socketID },
	m_socket{ socket },
	m_overlappedEx{}
{
}

ClientSocket::~ClientSocket()
{
}
