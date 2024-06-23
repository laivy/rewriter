#include "Stdafx.h"
#include "SocketManager.h"
#include "User.h"
#include "UserManager.h"

SocketManager::SocketManager() :
	m_hIOCP{ INVALID_HANDLE_VALUE },
	m_listenSocket{ INVALID_SOCKET },
	m_clientSocket{ INVALID_SOCKET },
	m_acceptBuffer{},
	m_overlappedEx{}
{
	WSADATA wsaData{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		assert(false && "FAIL WSAStartup");
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

	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "SOCKET MANAGER CREATE IOCP FAIL");
		return;
	}

	if (!::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_hIOCP, 0, 0))
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

	for (auto& thread : m_threads)
		thread = std::jthread{ std::bind_front(&SocketManager::Run, this) };
}

SocketManager::~SocketManager()
{
	for (auto& thread : m_threads)
		thread.request_stop();
	::WSACleanup();
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
	User* user{ nullptr };
	OVERLAPPEDEX* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_hIOCP, &ioSize, reinterpret_cast<unsigned long long*>(&user), reinterpret_cast<OVERLAPPED**>(&overlappedEx), 1000))
		{
			switch (overlappedEx->op)
			{
			case IOOP::ACCEPT:
				OnAccept();
				break;
			case IOOP::RECEIVE:
				if (ioSize > 0)
					OnReceive(user, ioSize);
				else
					OnDisconnect(user);
				break;
			}
			continue;
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 클라이언트에서 강제로 연결 끊음
			OnDisconnect(user);
			continue;
		case WAIT_TIMEOUT:
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

	// 유저 객체 생성
	auto socket{ std::make_shared<Socket>() };
	socket->socket = m_clientSocket;
	socket->overlappedEx.op = IOOP::RECEIVE;
	auto user{ std::make_shared<User>(socket) };
	if (auto um{ UserManager::GetInstance() })
		um->Register(user);

	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket->socket), m_hIOCP, reinterpret_cast<unsigned long long>(user.get()), 0);
	WSABUF wsaBuf{ static_cast<unsigned long>(socket->buffer.size()), socket->buffer.data() };
	DWORD flag{};
	::WSARecv(socket->socket, &wsaBuf, 1, 0, &flag, &socket->overlappedEx, nullptr);

	// 새로운 Accept 소켓 생성
	m_clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	m_acceptBuffer.fill(0);
	m_overlappedEx = {};
	::AcceptEx(m_listenSocket, m_clientSocket, m_acceptBuffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &m_overlappedEx);
}

void SocketManager::OnReceive(User* user, unsigned long ioSize)
{
	const auto& socket{ user->GetSocket() };
	if (socket->packet && socket->remainSize > 0)
	{
		socket->packet->EncodeBuffer(socket->buffer.data(), static_cast<Packet::size_type>(ioSize));
		socket->remainSize -= ioSize;
	}
	else
	{
		socket->packet = std::make_unique<Packet>(socket->buffer.data(), static_cast<Packet::size_type>(ioSize));

		Packet::size_type size{ 0 };
		std::memcpy(&size, socket->buffer.data(), sizeof(size));
		if (size > ioSize)
			socket->remainSize = size - ioSize;
	}

	if (socket->packet && socket->remainSize == 0)
	{
		socket->packet->SetOffset(0);
		user->OnPacket(*socket->packet);
		socket->packet.reset();
	}

	WSABUF wsaBuf{ static_cast<unsigned long>(socket->buffer.size()), socket->buffer.data() };
	DWORD flag{};
	::WSARecv(socket->socket, &wsaBuf, 1, 0, &flag, &socket->overlappedEx, nullptr);
}

void SocketManager::OnDisconnect(User* user)
{
	const auto& socket{ user->GetSocket() };
	::shutdown(socket->socket, SD_BOTH);
	::closesocket(socket->socket);

	if (auto um{ UserManager::GetInstance() })
		um->Unregister(user);
}