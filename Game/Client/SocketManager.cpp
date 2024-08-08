#include "Stdafx.h"
#include "App.h"
#include "SocketManager.h"
#include "Common/Socket.h"

SocketManager::SocketManager() :
	m_iocp{ INVALID_HANDLE_VALUE }
{
	WSADATA wsaData{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		assert(false && "WSA INIT FAIL");
		return;
	}

	m_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (!m_iocp)
	{
		assert(false && "CREATE IOCP HANDLE FAIL");
		return;
	}

	m_thread = std::jthread{ std::bind_front(&SocketManager::Run, this) };
}

SocketManager::~SocketManager()
{
	m_thread.request_stop();
	::CloseHandle(m_iocp);
	::WSACleanup();
}

void SocketManager::Register(ISocket* socket) const
{
	HANDLE iocp{ ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(static_cast<SOCKET>(*socket)), m_iocp, reinterpret_cast<unsigned long long>(socket), 0) };
	if (iocp != m_iocp)
		assert(false && "REGISTER SOCKET TO IOCP FAIL");
}

void SocketManager::Run(std::stop_token stoken)
{
	unsigned long ioSize{};
	ISocket* socket{};
	ISocket::OverlappedEx* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_iocp, &ioSize, reinterpret_cast<unsigned long long*>(&socket), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
		{
			if (!socket)
				continue;

			if (ioSize == 0)
			{
				socket->OnDisconnect();
				continue;
			}

			switch (overlappedEx->op)
			{
			case ISocket::IOOperation::Send:
				socket->OnSend(static_cast<Packet::Size>(ioSize));
				break;
			case ISocket::IOOperation::Receive:
				socket->OnReceive(static_cast<Packet::Size>(ioSize));
				break;
			}
			continue;
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 서버에서 강제로 연결 끊음
			if (socket)
				socket->OnDisconnect();
			continue;
		case ERROR_ABANDONED_WAIT_0: // IOCP 핸들 닫힘
			continue;
		default:
			assert(false && "IOCP ERROR");
			continue;
		}
	}
}