#include "Stdafx.h"
#include "LoginServer.h"
#include "ServerAcceptor.h"

ServerAcceptor::ServerAcceptor() :
	m_port{ 0 },
	m_iocp{ INVALID_HANDLE_VALUE },
	m_listenSocket{ INVALID_SOCKET },
	m_clientSocket{ INVALID_SOCKET },
	m_acceptBuffer{},
	m_overlappedEx{}
{
	Init();

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
		if (::WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "ACCEPT FAIL");
			return;
		}
	}

	m_thread = std::jthread{ std::bind_front(&ServerAcceptor::Run, this) };
}

ServerAcceptor::~ServerAcceptor()
{
	m_thread.request_stop();
	::CloseHandle(m_iocp);
}

void ServerAcceptor::Render()
{
	if (ImGui::Begin("SERVER MANAGER"))
	{
	}
	ImGui::End();
}

void ServerAcceptor::Init()
{
	// 서버 정보 로드
	auto root = Resource::Get(L"Center.dat");
	m_port = root->GetInt(L"Port");

	// 다른 서버들 로드
	auto load = [this, &root](const std::wstring& name, IServer::Type type)
		{
			for (const auto& [_, index] : *root->Get(name))
			{
				auto ip = index->GetString(L"IP");
				auto port = index->GetInt(L"Port");
				m_serverLists.emplace_back(type, ip, port);
			}
		};
	load(L"Login", IServer::Type::LOGIN);
	load(L"Game", IServer::Type::GAME);
}

void ServerAcceptor::Run(std::stop_token stoken)
{
	unsigned long ioSize{};
	IServer* server{ nullptr };
	OVERLAPPEDEX* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_iocp, &ioSize, reinterpret_cast<unsigned long long*>(&server), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
		{
			switch (overlappedEx->op)
			{
			case OVERLAPPEDEX::IOOP::ACCEPT:
				OnAccept();
				break;
			case OVERLAPPEDEX::IOOP::RECEIVE:
				if (ioSize > 0)
					OnReceive(server, static_cast<Packet::Size>(ioSize));
				else
					OnDisconnect(server);
				break;
			}
			continue;
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 클라이언트에서 강제로 연결 끊음
			OnDisconnect(server);
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

void ServerAcceptor::OnAccept()
{
	// 데이터에 있는지 확인
	SOCKADDR_IN* localAddr{};
	SOCKADDR_IN* remoteAddr{};
	int localAddrSize{};
	int remoteAddrSize{};

	::GetAcceptExSockaddrs(
		&m_acceptBuffer,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAddr),
		&localAddrSize,
		reinterpret_cast<SOCKADDR**>(&remoteAddr),
		&remoteAddrSize
	);

	// 클라이언트 소켓 특성을 리슨 소켓과 동일하게 설정
	if (::setsockopt(m_clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_listenSocket), sizeof(m_listenSocket)))
	{
		assert(false && "UPDATE ACCEPT CONTEXT FAIL");
		return;
	}

	// 서버 객체 생성
	auto server{ std::make_unique<LoginServer>() };
	auto& socket{ server->GetSocket() };
	socket.socket = m_clientSocket;
	socket.overlappedEx.op = OVERLAPPEDEX::IOOP::RECEIVE;

	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket.socket), m_iocp, reinterpret_cast<unsigned long long>(server.get()), 0);
	WSABUF wsaBuf{ static_cast<unsigned long>(socket.buffer.size()), socket.buffer.data() };
	DWORD flag{};
	::WSARecv(socket.socket, &wsaBuf, 1, 0, &flag, &socket.overlappedEx, nullptr);

	// 새로운 Accept 소켓 생성
	m_clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	m_acceptBuffer.fill(0);
	m_overlappedEx = {};
	::AcceptEx(m_listenSocket, m_clientSocket, m_acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &m_overlappedEx);

	m_servers.push_back(std::move(server));
}

void ServerAcceptor::OnReceive(IServer* server, Packet::Size ioSize)
{
	auto& socket{ server->GetSocket() };
	if (socket.packet && socket.remainSize > 0)
	{
		socket.packet->EncodeBuffer(socket.buffer.data(), static_cast<Packet::Size>(ioSize));
		socket.remainSize -= ioSize;
	}
	else
	{
		socket.packet = std::make_unique<Packet>(socket.buffer.data(), static_cast<Packet::Size>(ioSize));

		Packet::Size size{ 0 };
		std::memcpy(&size, socket.buffer.data(), sizeof(size));
		if (size > ioSize)
			socket.remainSize = size - ioSize;
	}

	if (socket.packet && socket.remainSize == 0)
	{
		socket.packet->SetOffset(0);
		//server->OnPacket(*socket.packet);
		socket.packet.reset();
	}

	WSABUF wsaBuf{ static_cast<unsigned long>(socket.buffer.size()), socket.buffer.data() };
	DWORD flag{};
	::WSARecv(socket.socket, &wsaBuf, 1, 0, &flag, &socket.overlappedEx, nullptr);
}

void ServerAcceptor::OnDisconnect(IServer* server)
{
	auto& socket{ server->GetSocket() };
	::shutdown(socket.socket, SD_BOTH);
	::closesocket(socket.socket);
	std::erase_if(m_servers, [server](const auto& s) { return s.get() == server; });
}