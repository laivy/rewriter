#include "Stdafx.h"
#include "CenterServer.h"
#include "SocketManager.h"

CenterServer::CenterServer()
{
	SocketManager::GetInstance()->Register(this);
	if (auto prop{ Resource::Get(L"Server.dat/LoginServer/CenterServer") })
	{
		auto ip{ prop->GetString(L"IP") };
		auto port{ prop->GetInt(L"Port") };
		Connect(ip, port);
	}
	else
	{
		assert(false && "CAN NOT FIND SERVER CONFIG");
		::PostQuitMessage(0);
	}
}

void CenterServer::OnConnect(bool success)
{
	ISocket::OnConnect(success);
	if (success)
	{
		Packet packet{ Protocol::ServerBasicInfo };
		packet.Encode(ISocket::Type::Login);
		Send(packet);
		return;
	}

	// 다시 연결
	if (auto prop{ Resource::Get(L"Server.dat/LoginServer/CenterServer") })
	{
		auto ip{ prop->GetString(L"IP") };
		auto port{ prop->GetInt(L"Port") };
		Connect(ip, port);
	}
}

void CenterServer::OnDisconnect()
{
	ISocket::OnDisconnect();

	// 다시 연결
	if (auto prop{ Resource::Get(L"Server.dat/LoginServer/CenterServer") })
	{
		auto ip{ prop->GetString(L"IP") };
		auto port{ prop->GetInt(L"Port") };
		Connect(ip, port);
	}
	else
	{
		assert(false && "CAN NOT FIND SERVER CONFIG");
		::PostQuitMessage(0);
	}
}

void CenterServer::OnComplete(Packet& packet)
{
}
