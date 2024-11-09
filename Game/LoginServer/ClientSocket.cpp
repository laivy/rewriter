#include "Stdafx.h"
#include "CenterServer.h"
#include "ClientSocket.h"
#include "User.h"
#include "UserManager.h"

ClientSocket::ClientSocket(SOCKET socket) :
	ISocket{ socket }
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

void ClientSocket::OnComplete(Packet& packet)
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
	case Packet::Type::RequestLogin:
		OnLoginRequest(packet);
		break;
	default:
		break;
	}
}

void ClientSocket::OnRegisterRequest(Packet& packet)
{
	if (auto center{ CenterServer::GetInstance() })
	{
		Packet response{ Packet::Type::RequestRegisterToCenter };
		response.EncodeBuffer(std::span{ packet.GetBuffer(), packet.GetSize() });
		center->Send(response);
	}
}

void ClientSocket::OnLoginRequest(Packet& packet)
{
	auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };

	if (auto center{ CenterServer::GetInstance() })
	{
		Packet outPacket{ Packet::Type::RequestLoginToCenter };
		outPacket.Encode(id, pw);
		center->Send(outPacket);
	}
}
