#include "Stdafx.h"
#include "CenterServer.h"
#include "ClientSocket.h"
#include "User.h"
#include "UserManager.h"

ClientSocket::ClientSocket(SOCKET socket)
{
	m_socket = socket;
}

ClientSocket::~ClientSocket()
{
	if (m_user)
	{
		if (auto um{ UserManager::GetInstance() })
			um->Unregister(m_user.get());
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
