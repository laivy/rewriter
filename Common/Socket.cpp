#include "Stdafx.h"
#include "Socket.h"
#include "Util.h"

ISocket::SendBuffer::SendBuffer() :
	overlappedEx{},
	size{}
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
	m_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	assert(m_socket != INVALID_SOCKET);
}

ISocket::ISocket(SOCKET socket) :
	m_socket{ socket },
	m_ip(INET_ADDRSTRLEN, '\0')
{
	// 소켓 아이피 주소 가져옴
	SOCKADDR_IN sockAddr{};
	int addrLen{ sizeof(sockAddr) };
	if (::getpeername(m_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), &addrLen) != SOCKET_ERROR)
	{
		::inet_ntop(sockAddr.sin_family, &sockAddr.sin_addr, m_ip.data(), m_ip.size());
		std::erase(m_ip, '\0');
	}
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

void ISocket::OnConnect(bool success)
{
	if (success)
		Receive();
}

void ISocket::OnDisconnect()
{
	Disconnect();
}

void ISocket::OnSend(OverlappedEx* overlappedEx)
{
	std::lock_guard lock{ m_mutex };
	auto it{ std::ranges::find_if(m_sendBuffers, [overlappedEx](const auto& sendBuffer) { return &sendBuffer.overlappedEx == overlappedEx; }) };
	if (it != m_sendBuffers.end())
		m_sendBuffers.erase(it);
}

void ISocket::OnReceive(Packet::Size ioSize)
{
	std::lock_guard lock{ m_mutex };

	// 조립 중인 패킷이 있는 경우는 버퍼 뒤에 붙임
	if (m_receiveBuffer.packet && m_receiveBuffer.remainPacketSize > 0)
	{
		m_receiveBuffer.packet->EncodeBuffer(std::span{ m_receiveBuffer.buffer.data(), ioSize });
		m_receiveBuffer.remainPacketSize -= ioSize;
		if (m_receiveBuffer.remainPacketSize < 0)
		{
			Disconnect();
			return;
		}
	}
	else
	{
		m_receiveBuffer.packet = std::make_unique<Packet>(std::span{ m_receiveBuffer.buffer.data(), ioSize });
		auto size{ m_receiveBuffer.packet->GetSize() };
		if (size > ioSize)
			m_receiveBuffer.remainPacketSize = size - ioSize;
	}

	// 패킷 완성
	if (m_receiveBuffer.packet && m_receiveBuffer.remainPacketSize == 0)
	{
		m_receiveBuffer.packet->SetOffset(sizeof(Packet::Size) + sizeof(Packet::Type));
		OnComplete(*m_receiveBuffer.packet);
		m_receiveBuffer.packet.reset();
	}

	Receive();
}

void ISocket::OnComplete(Packet& packet)
{
}

bool ISocket::Connect(std::wstring_view ip, unsigned short port)
{
	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	if (::bind(m_socket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR)
	{
		// 이미 바인드 되어 있는 경우는 예외
		if (::WSAGetLastError() != WSAEINVAL)
		{
			Disconnect();
			return false;
		}
	}

	GUID guid{ GUID WSAID_CONNECTEX };
	LPFN_CONNECTEX ConnectEx{ nullptr };
	DWORD bytes{};
	addr.sin_port = ::htons(port);
	::InetPtonW(AF_INET, ip.data(), &(addr.sin_addr.s_addr));
	if (::WSAIoctl(m_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &ConnectEx, sizeof(ConnectEx), &bytes, nullptr, nullptr) == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	// 연결 할 때 수신 버퍼를 사용함
	m_receiveBuffer.overlappedEx = OverlappedEx{ .op = IOOperation::Connect };
	if (!ConnectEx(m_socket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr), nullptr, 0, nullptr, &m_receiveBuffer.overlappedEx))
	{
		if (::WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "CONNECTEX FAIL");
			return false;
		}
	}

	m_ip = Util::wstombs(ip);
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

	std::lock_guard lock{ m_mutex };
	std::list<SendBuffer>{}.swap(m_sendBuffers);
	m_receiveBuffer = {};
	m_receiveBuffer.overlappedEx.op = IOOperation::Receive;
}

void ISocket::Send(Packet& packet)
{
	std::lock_guard lock{ m_mutex };

	auto& sendBuffer{ m_sendBuffers.emplace_back() };
	sendBuffer.overlappedEx.op = IOOperation::Send;
	sendBuffer.size = packet.GetSize();
	sendBuffer.buffer.reset(packet.Detach());

	WSABUF wsaBuf{ sendBuffer.size, sendBuffer.buffer.get() };
	if (::WSASend(m_socket, &wsaBuf, 1, nullptr, 0, &sendBuffer.overlappedEx, nullptr) == SOCKET_ERROR)
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			OnDisconnect();
			return;
		}
	}
}

void ISocket::Receive()
{
	std::lock_guard lock{ m_mutex };

	m_receiveBuffer.overlappedEx.op = IOOperation::Receive;
	WSABUF wsaBuf{ static_cast<unsigned long>(m_receiveBuffer.buffer.size()), m_receiveBuffer.buffer.data() };
	DWORD flag{};
	DWORD ioSize{};
	if (::WSARecv(m_socket, &wsaBuf, 1, &ioSize, &flag, &m_receiveBuffer.overlappedEx, nullptr) == SOCKET_ERROR)
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			Disconnect();
			return;
		}
	}
	else
	{
		OnReceive(static_cast<Packet::Size>(ioSize));
	}
}

bool ISocket::IsConnected() const
{
	return m_socket != INVALID_SOCKET;
}

std::string ISocket::GetIP() const
{
	return m_ip;
}
