#include "Stdafx.h"
#include "LoginServer.h"

LoginServer::LoginServer(SOCKET socket) :
	ISocket{ socket }
{
}

void LoginServer::OnComplete(Packet& packet)
{
	switch (packet.GetType())
	{
	case Packet::Type::RequestLoginToCenter:
	{
		auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };
		break;
	}
	default:
		break;
	}
}
