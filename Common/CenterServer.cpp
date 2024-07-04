#include "Stdafx.h"
#include "CenterServer.h"
#include "Util.h"

CenterServer::CenterServer(std::wstring_view config) :
	m_socket{ INVALID_SOCKET },
	m_port{ 0 },
	m_buffer{},
	m_remainSize{ 0 }
{
	auto root{ Resource::Get(config) };
	m_ip = root->GetString(L"Center/IP");
	m_port = root->GetInt(L"Center/Port");
	m_thread = std::jthread{ std::bind_front(&CenterServer::Run, this) };
}

bool CenterServer::IsConnected() const
{
	return m_socket != INVALID_SOCKET;
}

void CenterServer::Run(std::stop_token stoken)
{
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		assert(false && "CREATE SOCKET FAIL");
		return;
	}

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(m_port);
	::InetPtonW(AF_INET, m_ip.c_str(), &(addr.sin_addr.s_addr));

	// 연결될 때까지 1초마다 연결 시도
	while (::connect(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)))
		std::this_thread::sleep_for(1s);

	Packet packet{ Packet::Type::CLIENT_TryLogin };
	packet.Encode(1);
	packet.End();
	::send(m_socket, packet.GetBuffer(), packet.GetSize(), 0);

	while (!stoken.stop_requested())
	{
		int received{ ::recv(m_socket, m_buffer.data(), static_cast<int>(m_buffer.size()), 0) };
		if (received == SOCKET_ERROR)
			continue;

		if (received > 0)
			OnReceive(received);
		else
			OnDisconnect();
	}
}

void CenterServer::OnReceive(int received)
{
	if (m_packet && m_remainSize > 0)
	{
		m_packet->EncodeBuffer(m_buffer.data(), static_cast<Packet::size_type>(received));
		m_remainSize -= received;
	}
	else
	{
		m_packet = std::make_unique<Packet>(m_buffer.data(), static_cast<Packet::size_type>(received));

		Packet::size_type size{ 0 };
		std::memcpy(&size, m_buffer.data(), sizeof(size));
		if (size > received)
			m_remainSize = size - received;
	}

	if (m_packet && m_remainSize == 0)
	{
		m_packet->SetOffset(0);

		auto recv{ m_packet->Decode<int>() };

		Packet packet{ Packet::Type::LOGIN_TryLogin };
		packet.Encode(recv + 1);
		packet.End();
		::send(m_socket, packet.GetBuffer(), packet.GetSize(), 0);
		//OnPacket(*m_packet);
		m_packet.reset();
	}
}

void CenterServer::OnDisconnect()
{
	m_ip.clear();
	m_port = 0;
	::shutdown(m_socket, SD_BOTH);
	::closesocket(m_socket);
	m_socket = INVALID_SOCKET;
}
