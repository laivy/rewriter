#include "Stdafx.h"
#include "App.h"
#include "SocketManager.h"
#include "Shared/Socket.h"

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
	if (m_iocp != ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(static_cast<SOCKET>(*socket)), m_iocp, reinterpret_cast<unsigned long long>(socket), 0))
		assert(false && "REGISTER SOCKET TO IOCP FAIL");
}

void SocketManager::Run(std::stop_token stoken)
{
	DWORD ioSize{};
	ISocket* socket{};
	ISocket::OverlappedEx* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_iocp, &ioSize, reinterpret_cast<PULONG_PTR>(&socket), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
		{
			if (!overlappedEx)
				continue;

			if (ioSize == 0)
			{
				switch (overlappedEx->op)
				{
				case ISocket::IOOperation::Connect:
				{
					if (socket)
						socket->OnConnect(true);
					break;
				}
				default:
					break;
				}
				continue;
			}

			if (!socket)
				continue;

			switch (overlappedEx->op)
			{
			case ISocket::IOOperation::Send:
			{
				socket->OnSend(overlappedEx);
				break;
			}
			case ISocket::IOOperation::Receive:
			{
				socket->OnReceive(static_cast<Packet::Size>(ioSize));
				break;
			}
			default:
				assert(false && "INVALID SOCKET STATE");
				continue;
			}
			continue;
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 서버에서 강제로 연결 끊음
		{
			if (socket)
				socket->OnDisconnect();
			continue;
		}
		case ERROR_ABANDONED_WAIT_0: // IOCP 핸들 닫힘
		case ERROR_OPERATION_ABORTED: // IO 작업 취소됨
		{
			continue;
		}
		case ERROR_CONNECTION_REFUSED: // 연결 실패
		{
			if (socket)
				socket->OnConnect(false);
			continue;
		}
		default:
			assert(false && "IOCP ERROR");
			continue;
		}
	}
}
