#include "Stdafx.h"
#include "App.h"
#include "LocalSocket.h"

LocalSocket::LocalSocket() :
	m_socket{ INVALID_SOCKET },
	m_receiveBuffer{}
{
	m_receiveBuffer.overlappedEx.op = IOOperation::Receive;
}

LocalSocket::~LocalSocket()
{
	Disconnect();
}

void LocalSocket::OnDisconnect()
{
	Disconnect();
}

void LocalSocket::OnSend(Packet::Size size)
{
}

void LocalSocket::OnReceive(Packet::Size size)
{
	// 조립 중인 패킷이 있는 경우는 버퍼 뒤에 붙임
	if (m_receiveBuffer.packet && m_receiveBuffer.remainPacketSize > 0)
	{
		m_receiveBuffer.packet->EncodeBuffer(m_receiveBuffer.buffer.data(), size);
		m_receiveBuffer.remainPacketSize -= size;
		if (m_receiveBuffer.remainPacketSize < 0)
		{
			Disconnect();
			return;
		}
	}
	else
	{
		m_receiveBuffer.packet = std::make_unique<Packet>(m_receiveBuffer.buffer.data(), size);

		Packet::Size packetSize{ 0 };
		std::memcpy(&packetSize, m_receiveBuffer.buffer.data(), sizeof(packetSize));
		if (packetSize > size)
			m_receiveBuffer.remainPacketSize = packetSize - size;
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

bool LocalSocket::Connect(std::wstring_view ip, unsigned short port)
{
	m_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		auto err = ::WSAGetLastError();

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

void LocalSocket::Disconnect()
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

void LocalSocket::Send(Packet& packet)
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

void LocalSocket::Receive()
{
	WSABUF wsaBuf{ static_cast<unsigned long>(m_receiveBuffer.buffer.size()), m_receiveBuffer.buffer.data() };
	DWORD flag{};
	if (::WSARecv(m_socket, &wsaBuf, 1, 0, &flag, &m_receiveBuffer.overlappedEx, nullptr))
		Disconnect();
}

bool LocalSocket::IsConnected() const
{
	return m_socket != INVALID_SOCKET;
}
