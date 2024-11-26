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
	case Protocol::AccountRegisterRequest:
	{
		OnAccountRegisterRequest(packet);
		break;
	}
	default:
		assert(false);
		break;
	}
}

void ClientSocket::OnAccountRegisterRequest(Packet& packet)
{
	auto subType{ packet.Decode<AccountRegisterRequest>() };
	switch (subType)
	{
	case AccountRegisterRequest::CheckID:
	{
		auto id{ packet.Decode<std::wstring>() };
		Packet outPacket{ Protocol::AccountRegisterRequest };
		outPacket.Encode(subType, id);
		CenterServer::GetInstance()->Send(outPacket);
		break;
	}
	case AccountRegisterRequest::Request:
		break;
	default:
		assert(false);
		break;
	}
}

void ClientSocket::OnLoginRequest(Packet& packet)
{
}
