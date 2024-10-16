#include "Stdafx.h"
#include "ClientSocket.h"
#include "SocketManager.h"
#include "User.h"
#include "UserManager.h"
#include "Common/Socket.h"
#ifdef _IMGUI
#include "Common/ImguiEx.h"
#endif

SocketManager::SocketManager() :
	m_iocp{ INVALID_HANDLE_VALUE },
	m_listenSocket{ INVALID_SOCKET }
{
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
	if (!m_iocp)
	{
		assert(false && "SOCKET MANAGER CREATE IOCP FAIL");
		return;
	}

	if (::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_iocp, 0, 0) != m_iocp)
	{
		assert(false && "REGISTER IOCP FAIL");
		return;
	}

	// 첫번째 스레드는 클라이언트 연결, 나머지는 패킷 송수신 처리
	for (size_t i{ 0 }; i < m_threads.size(); ++i)
		m_threads[i] = std::jthread{ std::bind_front(i == 0 ? &SocketManager::Accept : &SocketManager::Run, this) };
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
#ifdef _IMGUI
	if (ImGui::Begin("SOCKET MANAGER"))
	{
	}
	ImGui::End();
#endif
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
	SOCKET acceptSocket{ INVALID_SOCKET };

	auto createAcceptSocket =
		[this, &acceptBuffer, &acceptOverlappedEx, &acceptSocket]()
		{
			acceptBuffer.fill(0);
			acceptOverlappedEx = {};
			acceptSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
			if (acceptSocket == INVALID_SOCKET)
			{
				assert(false && "CREATE CLIENT SOCKET FAIL");
				return false;
			}
			if (!::AcceptEx(m_listenSocket, acceptSocket, acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &acceptOverlappedEx) && ::WSAGetLastError() != WSA_IO_PENDING)
			{
				assert(false && "ACCEPT FAIL");
				return false;
			}
			return true;
		};

	if (!createAcceptSocket())
		return;

	// 클라이언트 연결 처리
	DWORD ioSize{};
	ISocket* socket{};
	ISocket::OverlappedEx* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (!::GetQueuedCompletionStatus(m_iocp, &ioSize, reinterpret_cast<PULONG_PTR>(&socket), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
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

		// 클라이언트 소켓 객체 생성
		std::shared_ptr<ISocket> clientSocket{ std::make_shared<ClientSocket>(acceptSocket) };
		Register(clientSocket.get());
		clientSocket->Receive();
		m_sockets.push_back(clientSocket);

		// 계속 Accept
		if (!createAcceptSocket())
			break;
	}
}

void SocketManager::Register(ISocket* socket) const
{
	HANDLE iocp{ ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(static_cast<SOCKET>(*socket)), m_iocp, reinterpret_cast<ULONG_PTR>(socket), 0) };
	if (iocp != m_iocp)
		assert(false && "REGISTER SOCKET TO IOCP FAIL");
}

void SocketManager::Disconnect(ISocket* socket)
{
	if (socket)
	{
		socket->OnDisconnect();
		std::erase_if(m_sockets, [socket](const auto& s) { return s.get() == socket; });
	}
}
