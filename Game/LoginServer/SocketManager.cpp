#include "Stdafx.h"
#include "SocketManager.h"
#include "User.h"
#include "UserManager.h"

SocketManager::SocketManager() :
	m_iocp{ INVALID_HANDLE_VALUE },
	m_listenSocket{ INVALID_SOCKET },
	m_clientSocket{ INVALID_SOCKET },
	m_acceptBuffer{},
	m_overlappedEx{}
{
	m_listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		assert(false && "CREATE LISTEN SOCKET FAIL");
		return;
	}

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(9000);
	addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	if (::bind(m_listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
	{
		assert(false && "LISTEN SOCKET BIND FAIL");
		return;
	}

	BOOL option{ TRUE };
	if (::setsockopt(m_listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<char*>(&option), sizeof(option)) == SOCKET_ERROR)
	{
		assert(false && "LISTEN SOCKET SETSOCKOPT FAIL");
		return;
	}

	if (::listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		assert(false && "LISTEN SOCKET LISTEN FAIL");
		return;
	}

	m_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_iocp == INVALID_HANDLE_VALUE)
	{
		assert(false && "SOCKET MANAGER CREATE IOCP FAIL");
		return;
	}

	if (!::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_iocp, 0, 0))
	{
		assert(false && "REGISTER IOCP FAIL");
		return;
	}

	m_clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (m_clientSocket == INVALID_SOCKET)
	{
		assert(false && "CREATE CLIENT SOCKET FAIL");
		return;
	}

	if (!::AcceptEx(m_listenSocket, m_clientSocket, m_acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &m_overlappedEx))
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			assert(false && "ACCEPT FAIL");
			return;
		}
	}

	for (auto& thread : m_threads)
		thread = std::jthread{ std::bind_front(&SocketManager::Run, this) };
}

SocketManager::~SocketManager()
{
	for (auto& thread : m_threads)
		thread.request_stop();
	::CloseHandle(m_iocp);
}

void SocketManager::Render()
{
	if (ImGui::Begin("SOCKET MANAGER"))
	{
	}
	ImGui::End();
}

void SocketManager::Run(std::stop_token stoken)
{
	unsigned long ioSize{};
	ClientSocket* socket{ nullptr };
	OVERLAPPEDEX* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_iocp, &ioSize, reinterpret_cast<unsigned long long*>(&socket), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
		{
			switch (overlappedEx->ioType)
			{
			case OVERLAPPEDEX::IOType::Accept:
				OnAccept();
				break;
			case OVERLAPPEDEX::IOType::Receive:
				if (ioSize > 0)
					socket->OnReceive(static_cast<Packet::Size>(ioSize));
				else
					Disconnect(socket);
				break;
			}
			continue;
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 클라이언트에서 강제로 연결 끊음
			Disconnect(socket);
			continue;
		case ERROR_ABANDONED_WAIT_0: // IOCP 핸들 닫힘
			continue;
		default:
			assert(false && "IOCP ERROR");
			continue;
		}
	}
}

void SocketManager::OnAccept()
{
	// 클라이언트 소켓 특성을 리슨 소켓과 동일하게 설정
	if (::setsockopt(m_clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_listenSocket), sizeof(m_listenSocket)))
	{
		assert(false && "UPDATE ACCEPT CONTEXT FAIL");
		return;
	}

	// 소켓 객체 생성
	auto socket{ std::make_shared<ClientSocket>(m_clientSocket) };

	// IOCP 등록
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_clientSocket), m_iocp, reinterpret_cast<unsigned long long>(socket.get()), 0);

	// 수신 상태로 설정
	socket->SetReceive();
	m_sockets.push_back(socket);

	// 새로운 Accept 소켓 생성
	m_clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	m_acceptBuffer.fill(0);
	m_overlappedEx = {};
	::AcceptEx(m_listenSocket, m_clientSocket, m_acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &m_overlappedEx);
}

void SocketManager::Disconnect(ClientSocket* socket)
{
	if (socket)
	{
		socket->OnDisconnect();
		std::erase_if(m_sockets, [socket](const auto& s) { return s.get() == socket; });
	}
}
