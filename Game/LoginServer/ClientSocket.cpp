#include "Stdafx.h"
#include "CenterServer.h"
#include "ClientSocket.h"
#include "User.h"
#include "UserManager.h"

ClientSocket::ClientSocket(SOCKET socket) :
	ISocket{ socket }
{
	SetType(Type::Client);
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
	case Protocol::Type::Login:
	{
		OnLoginRequest(packet);
		break;
	}
	case Protocol::Type::Register:
	{
		OnAccountRegister(packet);
		break;
	}
	default:
		assert(false);
		break;
	}
}

void ClientSocket::OnLoginRequest(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Login>() };
	switch (subType)
	{
	case Protocol::Login::Request:
	{
		auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };
		Packet outPacket{ Protocol::Type::Login };
		outPacket.Encode(Protocol::Login::Request, GetID(), id, pw);
		CenterServer::GetInstance()->Send(outPacket);
		break;
	}
	default:
		assert(false);
		break;
	}
}

void ClientSocket::OnAccountRegister(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Register>() };
	switch (subType)
	{
	case Protocol::Register::Check:
	{
		auto id{ packet.Decode<std::wstring>() };
		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::Check, GetID(), id);
		CenterServer::GetInstance()->Send(outPacket);
		break;
	}
	case Protocol::Register::Request:
	{
		auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };
		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::Request, GetID(), id, pw);
		CenterServer::GetInstance()->Send(outPacket);
		break;
	}
	default:
		assert(false);
		break;
	}
}
