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
	case Packet::Type::ServerBasicInfo:
	{
		auto type{ packet.Decode<ISocket::Type>() };
		SetType(type);
		return;
	}
	default:
		break;
	}

	switch (GetType())
	{
	case ISocket::Type::Login:
	{
		OnLoginPacket(packet);
		break;
	}
	default:
		Disconnect();
		break;
	}
}

void ServerSocket::OnLoginPacket(Packet& packet)
{
	switch (packet.GetType())
	{
	case Packet::Type::RequestLoginToCenter:
	{
		auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };
		break;
	}
	default:
		Disconnect();
		break;
	}
}
