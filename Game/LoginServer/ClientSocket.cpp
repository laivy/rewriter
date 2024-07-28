#include "Stdafx.h"
#include "CenterServer.h"
#include "ClientSocket.h"
#include "User.h"
#include "UserManager.h"

ClientSocket::ClientSocket(SOCKET socket) :
	m_socket{ socket },
	m_overlappedEx{},
	m_recvBuffer{},
	m_packetRemainSize{ 0 }
{
}

ClientSocket::~ClientSocket()
{
	if (m_user)
	{
		if (auto um{ UserManager::GetInstance() })
			um->Unregister(m_user.get());
	}
}

void ClientSocket::OnReceive(Packet::Size transferredSize)
{
	if (m_packet && m_packetRemainSize > 0)
	{
		m_packet->EncodeBuffer(m_recvBuffer.data(), transferredSize);
		m_packetRemainSize -= transferredSize;
	}
	else
	{
		m_packet = std::make_unique<Packet>(m_recvBuffer.data(), transferredSize);

		Packet::Size size{ 0 };
		std::memcpy(&size, m_recvBuffer.data(), sizeof(size));
		if (size > transferredSize)
			m_packetRemainSize = size - transferredSize;
	}

	// 패킷 완성
	if (m_packet && m_packetRemainSize == 0)
	{
		m_packet->SetOffset(0);
		OnPacket(*m_packet.get());
		m_packet.reset();
	}

	SetReceive();
}

void ClientSocket::OnDisconnect()
{
	if (m_socket != INVALID_SOCKET)
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

void ClientSocket::SetReceive()
{
	m_overlappedEx.ioType = OVERLAPPEDEX::IOType::Receive;

	WSABUF wsaBuf{ static_cast<unsigned long>(m_recvBuffer.size()), m_recvBuffer.data() };
	DWORD flag{};
	if (::WSARecv(m_socket, &wsaBuf, 1, 0, &flag, reinterpret_cast<OVERLAPPED*>(&m_overlappedEx), nullptr))
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
			assert(false && "RECV FAIL");
	}
}

void ClientSocket::OnPacket(Packet& packet)
{
	// 유저 객체가 있는 상태면 패킷 전달
	if (m_user)
	{
		m_user->OnPacket(packet);
		return;
	}

	switch (packet.GetType())
	{
	case Packet::Type::RequestRegister:
		OnRegisterRequest(packet);
		break;
	case Packet::Type::RequestLoginIn:
		OnLoginRequest(packet);
		break;
	}
}

void ClientSocket::OnRegisterRequest(Packet& packet)
{
	if (auto center{ CenterServer::GetInstance() })
	{
		Packet response{ Packet::Type::RequestRegisterToCenter };
		response.EncodeBuffer(packet.GetBuffer(), packet.GetSize());
		center->Send(response);
	}
}

void ClientSocket::OnLoginRequest(Packet& packet)
{
	auto [id, pw] { packet.Decode<std::string, std::string>() };

	Packet outPacket{ Packet::Type::LoginResult };
	outPacket.Encode(false);
	Send(outPacket);
}

void ClientSocket::Send(const Packet& packet) const
{
	::send(m_socket, packet.GetBuffer(), packet.GetSize(), 0);
}
