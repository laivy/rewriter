#include "Stdafx.h"
#include "ServerSocket.h"
#include "SocketManager.h"
#include "LoginServer.h"

ServerSocket::ServerSocket(SOCKET socket) :
	ISocket{ socket },
	m_type{ Type::None }
{
}

void ServerSocket::OnComplete(Packet& packet)
{
	if (packet.GetType() != Packet::Type::ServerBasicInfo)
		return;

	auto socketManager{ SocketManager::GetInstance() };
	if (!socketManager)
		return;

	auto type{ packet.Decode<int>() };
	switch (type)
	{
	case 123:
	{
		SOCKET socket{ Detach() };
		//socketManager->Swap(this, std::make_unique<LoginServer>(socket));
		return;
	}
	default:
		assert(false && "INVALID SERVER TYPE");
		break;
	}
}

ServerSocket::Type ServerSocket::GetType() const
{
	return m_type;
}
