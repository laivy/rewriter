#include "Stdafx.h"
#include "SocketManager.h"
#include "ServerSocket.h"
#include "Common/Socket.h"

SocketManager::SocketManager() :
	m_iocp{ INVALID_HANDLE_VALUE },
	m_port{ 0 }
{
	LoadConfig();

	WSADATA wsaData{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		assert(false && "WSA INIT FAIL");
		return;
	}

	m_listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		assert(false && "CREATE LISTEN SOCKET FAIL");
		return;
	}

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(m_port);
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

	if (::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_iocp, 0, 0) != m_iocp)
	{
		assert(false && "REGISTER IOCP FAIL");
		return;
	}

	// 첫번째 스레드는 서버 연결, 나머지는 패킷 송수신 처리
	for (size_t i = 0; i < m_threads.size(); ++i)
	{
		if (i == 0)
			m_threads[i] = std::jthread{ std::bind_front(&SocketManager::Accept, this) };
		else
			m_threads[i] = std::jthread{ std::bind_front(&SocketManager::Run, this) };
	}
}

SocketManager::~SocketManager()
{
	for (auto& thread : m_threads)
		thread.request_stop();
	::CloseHandle(m_iocp);
	::WSACleanup();
}

void SocketManager::Render()
{
	if (ImGui::Begin("SERVER MANAGER"))
	{
	}
	ImGui::End();
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
			if (socket)
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
		case ERROR_NETNAME_DELETED: // 클라이언트에서 강제로 연결 끊음
			if (socket)
				socket->OnDisconnect();
			continue;
		case ERROR_ABANDONED_WAIT_0: // IOCP 핸들 닫힘
		case ERROR_OPERATION_ABORTED:
		case ERROR_CONNECTION_ABORTED:
			continue;
		default:
			assert(false && "IOCP ERROR");
			continue;
		}
	}
}

void SocketManager::Accept(std::stop_token stoken)
{
	std::array<char, 64> acceptBuffer{};
	ISocket::OverlappedEx acceptOverlappedEx{};
	SOCKET acceptSocket{ ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED) };
	if (acceptSocket == INVALID_SOCKET)
	{
		assert(false && "CREATE CLIENT SOCKET FAIL");
		return;
	}

	if (!::AcceptEx(m_listenSocket, acceptSocket, acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &acceptOverlappedEx) && ::WSAGetLastError() != WSA_IO_PENDING)
	{
		assert(false && "ACCEPT FAIL");
		return;
	}

	unsigned long ioSize{};
	ISocket* socket{ nullptr };
	ISocket::OverlappedEx* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (!::GetQueuedCompletionStatus(m_iocp, &ioSize, reinterpret_cast<unsigned long long*>(&socket), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
			continue;
		if (!overlappedEx)
			continue;
		if (overlappedEx->op != ISocket::IOOperation::Connect)
			continue;

		// 소켓 옵션 설정
		if (::setsockopt(acceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_listenSocket), sizeof(m_listenSocket)))
		{
			assert(false && "UPDATE ACCEPT CONTEXT FAIL");
			continue;
		}

		// 서버 소켓 객체 생성
		auto serverSocket{ std::make_shared<ServerSocket>(acceptSocket) };
		Register(serverSocket.get());
		serverSocket->Receive();
		m_sockets.push_back(serverSocket);

		// 계속 Accept
		acceptBuffer.fill(0);
		acceptOverlappedEx = {};
		acceptSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		if (acceptSocket == INVALID_SOCKET)
		{
			assert(false && "CREATE SOCKET FAIL");
			continue;
		}
		if (!::AcceptEx(m_listenSocket, acceptSocket, acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &acceptOverlappedEx) && ::WSAGetLastError() != WSA_IO_PENDING)
			assert(false && "ACCEPT FAIL");
	}
}

void SocketManager::LoadConfig()
{
	//// 서버 정보 로드
	//auto root = Resource::Get(L"Center.dat");
	//m_port = root->GetInt(L"Port");

	//// 다른 서버들 로드
	//auto load = [this, &root](const std::wstring& name, IServer::Type type)
	//	{
	//		for (const auto& child : root->Get(name)->GetChildren())
	//		{
	//			auto ip = child->GetString(L"IP");
	//			auto port = child->GetInt(L"Port");
	//			//m_serverLists.emplace_back(type, ip, port);
	//		}
	//	};
	//load(L"Login", IServer::Type::LOGIN);
	//load(L"Game", IServer::Type::GAME);
}

void SocketManager::Register(ISocket* socket) const
{
}

void SocketManager::Disconnect(ISocket* socket)
{
	if (socket)
	{
		socket->OnDisconnect();
		std::erase_if(m_sockets, [socket](const auto& s) { return s.get() == socket; });
	}
}
