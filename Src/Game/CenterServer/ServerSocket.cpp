#include "Stdafx.h"
#include "ServerSocket.h"

ServerSocket::ServerSocket(SOCKET socket) :
	ISocket{ socket }
{
}

void ServerSocket::OnComplete(Packet& packet)
{
	switch (packet.GetType())
	{
	case Protocol::Type::Initialize:
	{
		auto type{ packet.Decode<Type>() };
		SetType(type);
		return;
	}
	default:
		break;
	}

	switch (GetType())
	{
	case Type::Login:
	{
		OnLoginServerPacket(packet);
		break;
	}
	default:
		Disconnect();
		break;
	}
}
