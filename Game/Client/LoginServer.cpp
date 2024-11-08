#include "Stdafx.h"
#include "LoginServer.h"
#include "SocketManager.h"

LoginServer::LoginServer()
{
	SocketManager::GetInstance()->Register(this);
	if (!Connect(L"127.0.0.1", 9000))
	{
		::PostQuitMessage(0);
		return;
	}
}

void LoginServer::OnConnect(bool success)
{
}

void LoginServer::OnComplete(Packet& packet)
{
	OnPacket.Notify(packet);
}
