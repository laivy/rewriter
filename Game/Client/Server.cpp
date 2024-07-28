#include "Stdafx.h"
#include "Server.h"

IServer::IServer(Type type) :
	m_type{ type }
{
}

IServer::~IServer()
{
	::shutdown(m_socket.socket, SD_BOTH);
	::closesocket(m_socket.socket);
}

bool IServer::Connect(std::wstring_view ip, unsigned short port, HANDLE iocp)
{
	SOCKET s{ ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED) };
	if (s == INVALID_SOCKET)
	{
		assert(false && "CREATE SOCKET FAIL");
		return false;
	}
	m_socket.socket = s;

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(port);
	::InetPtonW(AF_INET, ip.data(), &(addr.sin_addr.s_addr));
	if (::WSAConnect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), nullptr, nullptr, nullptr, nullptr))
	{
		assert(false && "CONNECT FAIL");
		return false;
	}

	if (!::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), iocp, reinterpret_cast<unsigned long long>(this), 0))
	{
		assert(false && "REGISTER IOCP FAIL");
		return false;
	}

	WSABUF wsaBuf{ static_cast<unsigned long>(m_socket.buffer.size()), m_socket.buffer.data() };
	DWORD flag{};
	::WSARecv(m_socket.socket, &wsaBuf, 1, 0, &flag, &m_socket.overlappedEx, nullptr);
	return true;
}

void IServer::SendPacket(const Packet& packet) const
{
	if (m_socket.socket != INVALID_SOCKET)
		::send(m_socket.socket, packet.GetBuffer(), packet.GetSize(), 0);
}

IServer::Type IServer::GetType() const
{
	return m_type;
}

SocketEx& IServer::GetSocket()
{
	return m_socket;
}