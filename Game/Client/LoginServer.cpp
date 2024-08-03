#include "Stdafx.h"
#include "LoginServer.h"
#include "SocketManager.h"

LoginServer::LoginServer()
{
	// 로그인 서버 연결 실패하면 클라이언트 종료
	if (!Connect(L"127.0.0.1", 9000))
		::PostQuitMessage(0);
}