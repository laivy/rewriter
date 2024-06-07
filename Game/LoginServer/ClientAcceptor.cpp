#include "Stdafx.h"
#include "ClientAcceptor.h"
#include "User.h"

ClientAcceptor::ClientAcceptor() :
	m_isActive{ false },
	m_acceptSocket{ INVALID_SOCKET },
	m_hIOCP{ INVALID_HANDLE_VALUE },
	m_overlappedEx{},
	m_clientSocketID{}
{
	Init();
}

ClientAcceptor::~ClientAcceptor()
{
	m_isActive = false;
}

void ClientAcceptor::Render()
{
	ImGui::Begin("AcceptThread");
	if (ImGui::BeginTabBar("Tab"))
	{
		if (ImGui::BeginTabItem("Info"))
		{
			ImGui::TextUnformatted("Socket");
			ImGui::SameLine(100);
			ImGui::TextUnformatted(":");
			ImGui::SameLine();

			std::string str{};
			ImVec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
			if (m_isActive)
			{
				str = "Open";
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				str = "Close";
				color = { 1.0f, 0.0f, 0.0f, 1.0f };
			}
			ImGui::TextColored(color, str.c_str());

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Log"))
		{
			if (ImGui::BeginChild("LogView"))
			{
				std::unique_lock lock{ m_logMutex };
				for (const auto& log : m_logs)
				{
					ImGui::TextUnformatted(log.c_str());
				}
				lock.unlock();
				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
					ImGui::SetScrollHereY(1.0f);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();
}

void ClientAcceptor::Init()
{
	WSADATA wsaData{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		assert(false && "FAIL WSAStartup");
		return;
	}

	m_acceptSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_acceptSocket == INVALID_SOCKET)
	{
		assert(false && "FAIL WSASocket - listenSocket");
		return;
	}

	SOCKADDR_IN serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = ::htons(9000);
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	if (::bind(m_acceptSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		assert(false && "FAIL bind");
		return;
	}

	BOOL on{ TRUE };
	if (::setsockopt(m_acceptSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<char*>(std::addressof(on)), sizeof(on)) == SOCKET_ERROR)
	{
		assert(false && "FAIL setsockopt");
		return;
	}

	if (::listen(m_acceptSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		assert(false && "FAIL listen");
		return;
	}

	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "FAIL CreateIoCompletionPort()");
		return;
	}

	if (!::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_acceptSocket), m_hIOCP, 0, 0))
	{
		assert(false && "FAIL CreateIoCompletionPort()");
		return;
	}

	SOCKET clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET)
	{
		assert(false && "FAIL WSASocket() - acceptSocket");
		return;
	}

	m_overlappedEx = {};
	m_overlappedEx.op = IOOperation::ACCEPT;
	m_overlappedEx.socket = clientSocket;
	if (!::AcceptEx(m_acceptSocket, m_overlappedEx.socket, m_overlappedEx.buffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &m_overlappedEx))
	{
		if (::WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "FAIL AcceptEx()");
			return;
		}
	}

	m_isActive = true;
	for (auto& thread : m_threads)
		thread = std::jthread{ &ClientAcceptor::Run, this };
}

void ClientAcceptor::Run()
{
	DWORD ioSize{};
	size_t socketID{};
	OVERLAPPEDEX* overlappedEx{};

	while (m_isActive)
	{
		if (!::GetQueuedCompletionStatus(m_hIOCP, &ioSize, reinterpret_cast<PULONG_PTR>(&socketID), reinterpret_cast<OVERLAPPED**>(&overlappedEx), SOCKET_TIMEOUT_MILLISEC))
		{
			int error{ ::WSAGetLastError() };
			switch (error)
			{
			case ERROR_NETNAME_DELETED: // 클라이언트에서 강제로 연결 끊음
				OnDisconnect(socketID);
				continue;
			case WAIT_TIMEOUT:
				continue;
			default:
				assert(false && "FAIL GetQueuedCompletionStatus()");
				continue;
			}
		}

		switch (overlappedEx->op)
		{
		case IOOperation::ACCEPT:
		{
			OnAccept(overlappedEx);
			break;
		}
		case IOOperation::RECV:
		{
			if (ioSize > 0)
				OnReceive(socketID);
			else
				OnDisconnect(socketID);
			break;
		}
		default:
			assert(false && "INVALID IO OPERATION");
			break;
		}
	}
}

void ClientAcceptor::OnAccept(OVERLAPPEDEX* overlappedEx)
{
	// 연결된 클라이언트 정보를 로그로 출력
	SOCKADDR_IN* localAddr{};
	SOCKADDR_IN* remoteAddr{};
	int localAddrSize{};
	int remoteAddrSize{};

	::GetAcceptExSockaddrs(
		overlappedEx->buffer,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAddr),
		&localAddrSize,
		reinterpret_cast<SOCKADDR**>(&remoteAddr),
		&remoteAddrSize
	);

	std::array<std::string, 2> buffer;
	buffer[0].resize(16);
	buffer[1].resize(16);
	::inet_ntop(AF_INET, &localAddr->sin_addr, buffer[0].data(), sizeof(buffer[0]));
	::inet_ntop(AF_INET, &remoteAddr->sin_addr, buffer[1].data(), sizeof(buffer[1]));

	std::string log{ std::format("[hh:mm:ss] {} Connected", buffer[1].c_str()) };
	if (std::unique_lock lock{ m_logMutex })
		m_logs.push_back(log);

	if (::setsockopt(overlappedEx->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_acceptSocket), sizeof(m_acceptSocket)))
	{
		assert(false && "ACCEPT FAIL - setsockopt");
		return;
	}

	// 해당 소켓을 IOCP에 등록
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_overlappedEx.socket), m_hIOCP, m_clientSocketID, 0);

	// ClientSocket 추가
	ClientSocket clientSocket{ m_clientSocketID, m_overlappedEx.socket };
	++m_clientSocketID;

	// 해당 소켓을 수신 상태로 변경
	DWORD flag{};
	if (::WSARecv(clientSocket.socket, &clientSocket.overlappedEx.wsaBuf, 1, 0, &flag, &clientSocket.overlappedEx, NULL))
	{
		if (::WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "ACCEPT FAIL - WSARecv");
			return;
		}
	}
	clientSocket.overlappedEx.op = IOOperation::RECV;

	if (std::unique_lock lock{ m_socketMutex })
		m_clientSockets.emplace(clientSocket.socketID, std::move(clientSocket));

	// 새로운 Accept 소켓 생성
	overlappedEx->socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	::AcceptEx(
		m_acceptSocket,
		m_overlappedEx.socket,
		m_overlappedEx.buffer,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		nullptr,
		&m_overlappedEx
	);
}

void ClientAcceptor::OnReceive(size_t socketID)
{
	std::lock_guard lock{ m_socketMutex };
	if (!m_clientSockets.contains(socketID))
		return;

	const auto& socket{ m_clientSockets.at(socketID) };
	Packet packet{ socket.overlappedEx.wsaBuf.buf };
	auto [s]{ packet.Decode<std::wstring>() };

	int i = 0;
}

void ClientAcceptor::OnDisconnect(size_t socketID)
{
	std::lock_guard lock{ m_socketMutex };
	if (!m_clientSockets.contains(socketID))
		return;

	auto& clientSocket{ m_clientSockets.at(socketID) };
	::closesocket(clientSocket.socket);
	m_clientSockets.erase(socketID);

	if (std::unique_lock lock{ m_logMutex })
		m_logs.push_back("[hh:mm:ss] Disconnect");
}
