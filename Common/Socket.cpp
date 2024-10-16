#include "Stdafx.h"
#include "Socket.h"
#if defined _CLIENT
#include "Game/Client/App.h"
#elif defined _CENTER_SERVER
#include "Game/CenterServer/App.h"
#elif defined _LOGIN_SERVER
#include "Game/LoginServer/App.h"
#endif

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
	Disconnect();
}

void ISocket::OnSend(Packet::Size ioSize)
{
}

void ISocket::OnReceive(Packet::Size ioSize)
{
	// 조립 중인 패킷이 있는 경우는 버퍼 뒤에 붙임
	if (m_receiveBuffer.packet && m_receiveBuffer.remainPacketSize > 0)
	{
		m_receiveBuffer.packet->EncodeBuffer(m_receiveBuffer.buffer.data(), ioSize);
		m_receiveBuffer.remainPacketSize -= ioSize;
		if (m_receiveBuffer.remainPacketSize < 0)
		{
			Disconnect();
			return;
		}
	}
	else
	{
		m_receiveBuffer.packet = std::make_unique<Packet>(m_receiveBuffer.buffer.data(), ioSize);

		Packet::Size packetSize{ 0 };
		std::memcpy(&packetSize, m_receiveBuffer.buffer.data(), sizeof(packetSize));
		if (packetSize > ioSize)
			m_receiveBuffer.remainPacketSize = packetSize - ioSize;
	}

	// 패킷 완성
	if (m_receiveBuffer.packet && m_receiveBuffer.remainPacketSize == 0)
	{
		m_receiveBuffer.packet->SetOffset(0);
		App::OnPacket.Notify(*m_receiveBuffer.packet);
		m_receiveBuffer.packet.reset();
	}

	Receive();
}

bool ISocket::Connect(std::wstring_view ip, unsigned short port)
{
	m_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		assert(false && "CREATE SOCKET FAIL");
		return false;
	}

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(port);
	::InetPtonW(AF_INET, ip.data(), &(addr.sin_addr.s_addr));
	if (::WSAConnect(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), nullptr, nullptr, nullptr, nullptr))
	{
		assert(false && "CONNECT FAIL");
		Disconnect();
		return false;
	}
	return true;
}

void ISocket::Disconnect()
{
	if (m_socket != INVALID_SOCKET)
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	std::vector<SendBuffer>{}.swap(m_sendBuffers);
	m_receiveBuffer = {};
	m_receiveBuffer.overlappedEx.op = IOOperation::Receive;
}

void ISocket::Send(Packet& packet)
{
	SendBuffer sendBuffer{};
	sendBuffer.overlappedEx.op = IOOperation::Send;
	sendBuffer.size = packet.GetSize();
	sendBuffer.buffer.reset(packet.Detach());

	WSABUF wsaBuf{ sendBuffer.size, sendBuffer.buffer.get() };
	if (::WSASend(m_socket, &wsaBuf, 1, 0, 0, &sendBuffer.overlappedEx, nullptr) && ::WSAGetLastError() != WSA_IO_PENDING)
	{
		Disconnect();
		return;
	}

	m_sendBuffers.push_back(std::move(sendBuffer));
}

void ISocket::Receive()
{
	m_receiveBuffer.overlappedEx.op = IOOperation::Receive;
	WSABUF wsaBuf{ static_cast<unsigned long>(m_receiveBuffer.buffer.size()), m_receiveBuffer.buffer.data() };
	DWORD flag{};
	if (::WSARecv(m_socket, &wsaBuf, 1, 0, &flag, &m_receiveBuffer.overlappedEx, nullptr) && ::WSAGetLastError() != WSA_IO_PENDING)
		Disconnect();
}

bool ISocket::IsConnected() const
{
	return m_socket != INVALID_SOCKET;
}
