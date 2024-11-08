#include "Stdafx.h"
#include "ServerSocket.h"

ServerSocket::ServerSocket(SOCKET socket) :
	ISocket{ socket },
	m_type{ Type::None }
{
}

ServerSocket::~ServerSocket()
{
	m_connectThread.request_stop();
}

void ServerSocket::OnConnect()
{
}

ServerSocket::Type ServerSocket::GetType() const
{
	return m_type;
}

void ServerSocket::Connect(const std::wstring& ip, unsigned short port)
{
	m_connectThread = std::jthread{
		[this, ip, port](std::stop_token stoken)
		{
			while (!stoken.stop_requested())
			{
				if (ISocket::Connect(ip, port))
					break;

				std::this_thread::sleep_for(std::chrono::seconds{ 10 });
			}
			OnConnect();
		}
	};
}
