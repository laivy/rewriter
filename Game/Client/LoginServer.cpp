#include "Stdafx.h"
#include "LoginServer.h"
#include "SocketManager.h"

LoginServer::LoginServer()
{
	// 로그인 서버 연결 실패하면 클라이언트 종료
	if (!Connect(L"127.0.0.1", 9000))
	{
		::PostQuitMessage(0);
		return;
	}

	if (!SocketManager::IsInstanced())
	{
		::PostQuitMessage(0);
		return;
	}

	SocketManager::GetInstance()->Register(this);
}

void LoginServer::OnPacket(Packet& packet)
{
	switch (packet.GetType())
	{
	default:
		break;
	}
}

void LoginServer::OnDisconnect()
{
	ISocket::OnDisconnect();
	::MessageBox(NULL, L"Disconnect from Login Server", L"Rewriter", MB_OK);
	::PostQuitMessage(0);
}
