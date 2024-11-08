#include "Stdafx.h"
#include "CenterServer.h"
#include "SocketManager.h"

CenterServer::CenterServer() :
	m_config{ Resource::Get(L"Server.dat/LoginServer/CenterServer") }
{
	if (!m_config)
	{
		assert(false && "CAN NOT FIND SERVER CONFIG");
		::PostQuitMessage(0);
		return;
	}
	Connect(m_config->GetString(L"IP"), m_config->GetInt(L"Port"));
}

void CenterServer::OnConnect()
{
	SocketManager::GetInstance()->Register(this);
}

void CenterServer::OnDisconnect()
{
	ISocket::OnDisconnect();

	// 다시 연결
	Connect(m_config->GetString(L"IP"), m_config->GetInt(L"Port"));
}

void CenterServer::OnComplete(Packet& packet)
{
	switch (packet.GetType())
	{
	case Packet::Type::LoginResultToLogin:
		break;
	default:
		break;
	}
}
