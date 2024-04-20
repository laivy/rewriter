#include "Stdafx.h"
#include "LoginServer.h"

LoginServer::LoginServer() : 
	m_isConnected{ false },
	m_socket{ INVALID_SOCKET }
{
}

LoginServer::~LoginServer()
{
	if (m_isConnected)
	{
		m_isConnected = false;
		::closesocket(m_socket);
	}
}

bool LoginServer::Connect()
{
	if (m_isConnected)
		return true;

	if (m_socket != INVALID_SOCKET)
		::closesocket(m_socket);

	std::string ip{ "127.0.0.1" };
	std::string port{ "9000" };

	WSADATA wsa{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	// socket 생성
	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		return false;

	// connect
	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(std::stoi(port));
	::inet_pton(AF_INET, ip.c_str(), &(addr.sin_addr.s_addr));

	if (::connect(m_socket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)))
		return false;

	m_isConnected = true;
	m_thread = std::jthread{ &LoginServer::Run, this };
	return true;
}

void LoginServer::Send(const Packet& packet)
{
	if (!m_isConnected)
		return;

	::send(m_socket, packet.GetBuffer(), packet.GetSize(), 0);
}

bool LoginServer::IsConnected() const
{
	return m_isConnected;
}

void LoginServer::Run()
{
	std::array<char, 512> buffer{};
	while (m_isConnected)
	{
		int received{ ::recv(m_socket, buffer.data(), static_cast<int>(buffer.size()), 0) };
		if (received == 0)
			break;
	}
}
