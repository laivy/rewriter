#include "stdafx.h"
#include "Server.h"

std::map<int, std::unique_ptr<IServer>> g_servers;

IServer::IServer() :
	m_socket{ INVALID_SOCKET }
{
}

IServer::~IServer()
{
	Disconnect();
}

bool IServer::IsConnected() const
{
	return m_socket != INVALID_SOCKET;
}

void IServer::Connect(std::string_view ip, int port)
{
	WSADATA wsaData{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		assert(false && "FAIL WSAStartup");
		return;
	}

	SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);

	BOOL option{ TRUE };
	::setsockopt(socket, SOL_SOCKET, TCP_NODELAY, reinterpret_cast<char*>(&option), sizeof(option));

	SOCKADDR_IN sockAddr{};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = ::htons(port);
	::inet_pton(AF_INET, ip.data(), &(sockAddr.sin_addr.s_addr));
	if (::connect(socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr)))
	{
		assert(false && "FAIL connect");
		return;
	}

	m_socket = socket;
}

void IServer::Disconnect()
{
	if (m_socket != INVALID_SOCKET)
	{
		::closesocket(m_socket);
		::WSACleanup();
	}
	m_socket = INVALID_SOCKET;
}

void IServer::Send(const Packet& packet)
{
	if (m_socket == INVALID_SOCKET)
		return;

	::send(m_socket, packet.GetBuffer(), packet.GetSize(), 0);
}

void LoginServer::Connect()
{
	IServer::Connect(IP, PORT);
}