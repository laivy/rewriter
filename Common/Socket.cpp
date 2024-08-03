#include "Stdafx.h"
#include "Socket.h"

ISocket::SendBuffer::SendBuffer() :
	overlappedEx{},
	size{ 0 }
{
}

ISocket::SendBuffer::SendBuffer(SendBuffer&& other) noexcept :
	overlappedEx{ other.overlappedEx },
	buffer{ std::move(other.buffer) },
	size{ other.size }
{
	other.size = 0;
}

ISocket::SendBuffer& ISocket::SendBuffer::operator=(SendBuffer&& other) noexcept
{
	if (this != &other)
	{
		overlappedEx = other.overlappedEx;
		buffer = std::move(other.buffer);
		size = other.size;
		other.size = 0;
	}
	return *this;
}

ISocket::ISocket() :
	m_socket{ INVALID_SOCKET }
{
}

ISocket::~ISocket()
{
	if (m_socket != INVALID_SOCKET)
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

ISocket::operator SOCKET()
{
	return m_socket;
}

void ISocket::OnDisconnect()
{
}

void ISocket::OnSend(Packet::Size ioSize)
{
}

void ISocket::OnReceive(Packet::Size ioSize)
{
}

void ISocket::Send(const Packet& packet)
{
}