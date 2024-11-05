#include "Stdafx.h"
#include "ClientSocket.h"
#include "SocketManager.h"
#include "User.h"
#include "UserManager.h"
#include "Common/Socket.h"
#ifdef _IMGUI
#include "Common/ImguiEx.h"
#include "Common/Time.h"
#endif

SocketManager::SocketManager() :
	m_iocp{ INVALID_HANDLE_VALUE },
	m_listenSocket{ INVALID_SOCKET },
	m_acceptSocket{ INVALID_SOCKET },
	m_acceptBuffer{},
	m_acceptOverlappedEx{}
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

	for (auto& thread : m_threads)
		thread = std::jthread{ std::bind_front(&SocketManager::Run, this) };

	Accept();
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
	if (ImGui::Begin("Socket Manager"))
	{
		for (const auto& log : m_logs)
			ImGui::TextUnformatted(log.c_str());
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
			if (ioSize == 0 && overlappedEx && overlappedEx->op == ISocket::IOOperation::Connect)
			{
				OnConnect();
				continue;
			}

			if (!socket)
				continue;

			if (ioSize == 0)
			{
				Disconnect(socket);
				continue;
			}

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
			{
				assert(false && "INVALID SOCKET STATE");
				Disconnect(socket);
				continue;
			}
			}
			continue;
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 클라이언트에서 강제로 연결 끊음
		{
			Disconnect(socket);
			continue;
		}
		case ERROR_ABANDONED_WAIT_0: // IOCP 핸들 닫힘
		case ERROR_OPERATION_ABORTED: // IO 작업 취소됨
		{
			continue;
		}
		default:
		{
			assert(false && "IOCP ERROR");
			Disconnect(socket);
			continue;
		}
		}
	}
}

void SocketManager::OnConnect()
{
	std::lock_guard lock{ m_acceptMutex };

	// 소켓 옵션 설정
	if (::setsockopt(m_acceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_listenSocket), sizeof(m_listenSocket)))
	{
		assert(false && "UPDATE ACCEPT CONTEXT FAIL");
		return;
	}

	auto& clientSocket{ m_sockets.emplace_back(std::make_shared<ClientSocket>(m_acceptSocket)) };
	Register(clientSocket.get());
	clientSocket->Receive();

#ifdef _IMGUI
	SOCKADDR_IN* localAddr{};
	SOCKADDR_IN* remoteAddr{};
	int localAddrLen{};
	int remoteAddrLen{};
	::GetAcceptExSockaddrs(
		&m_acceptBuffer,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<sockaddr**>(&localAddr),
		&localAddrLen,
		reinterpret_cast<sockaddr**>(&remoteAddr),
		&remoteAddrLen
	);

	std::string ip(INET_ADDRSTRLEN, '\0');
	::inet_ntop(AF_INET, &remoteAddr->sin_addr, ip.data(), ip.size());
	std::erase(ip, '\0');
	Logging(std::format("Accept Client {}:{}", ip, remoteAddr->sin_port));
#endif

	Accept();
}

void SocketManager::Accept()
{
	std::lock_guard lock{ m_acceptMutex };

	m_acceptBuffer.fill(0);
	m_acceptOverlappedEx = {};
	m_acceptSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (m_acceptSocket == INVALID_SOCKET)
	{
		assert(false && "CREATE ACCEPT SOCKET FAIL");
		return;
	}

	if (!::AcceptEx(m_listenSocket, m_acceptSocket, m_acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &m_acceptOverlappedEx))
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			assert(false && "ACCEPT FAIL");
			return;
		}
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

#ifdef _IMGUI
void SocketManager::Logging(const std::string& log)
{
	Time now{ Time::Now() };
	std::string prefix{ std::format("[{}-{:02}-{:02} {:02}:{:02}:{:02}] ", now.Year(), now.Month(), now.Day(), now.Hour(), now.Min(), now.Sec()) };
	m_logs.push_back(prefix + log);
}
#endif
