#include "Stdafx.h"
#include "Acceptor.h"
#include "App.h"
#include "SocketManager.h"
#include "User.h"
#include "UserManager.h"

Acceptor::Acceptor() :
	m_isActive{ false },
	m_hIOCP{ INVALID_HANDLE_VALUE },
	m_listenSocket{ INVALID_SOCKET },
	m_clientSocket{ INVALID_SOCKET },
	m_buffer{},
	m_overlapped{},
	std::jthread{ &Acceptor::Run, this }
{
}

Acceptor::~Acceptor()
{
	m_isActive = false;
}

void Acceptor::Render()
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

void Acceptor::Run()
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

	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "CREATE IOCP FAIL");
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

	if (!::AcceptEx(m_listenSocket, m_clientSocket, m_buffer.data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, nullptr, &m_overlapped))
	{
		if (::WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "ACCEPT FAIL");
			return;
		}
	}
	m_isActive = true;

	unsigned long ioSize{};
	std::uint64_t completionKey{};
	OVERLAPPED* overlapped{};
	while (m_isActive)
	{
		if (::GetQueuedCompletionStatus(m_hIOCP, &ioSize, &completionKey, &overlapped, SOCKET_TIMEOUT_MILLISEC))
			OnAccept();
	}
}

void Acceptor::OnAccept()
{
	if (::setsockopt(m_clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_listenSocket), sizeof(m_listenSocket)))
	{
		assert(false && "UPDATE ACCEPT CONTEXT FAIL");
		return;
	}

	// 소켓, 유저 등록
	auto socket{ std::make_shared<Socket>() };
	socket->socket = m_clientSocket;
	auto user{ std::make_shared<User>(socket) };
	if (auto um{ UserManager::GetInstance() })
		um->Register(user);
	if (auto sm{ SocketManager::GetInstance() })
		sm->Register(user, socket);

	// 새로운 Accept 소켓 생성
	m_overlapped = {};
	m_buffer.fill(0);
	m_clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	::AcceptEx(
		m_listenSocket,
		m_clientSocket,
		m_buffer.data(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		nullptr,
		&m_overlapped
	);

	// 연결된 클라이언트 정보 출력
	SOCKADDR_IN* localAddr{};
	SOCKADDR_IN* remoteAddr{};
	int localAddrSize{};
	int remoteAddrSize{};

	::GetAcceptExSockaddrs(
		m_buffer.data(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAddr),
		&localAddrSize,
		reinterpret_cast<SOCKADDR**>(&remoteAddr),
		&remoteAddrSize
	);

	std::string buffer(16, '\0');
	::inet_ntop(AF_INET, &remoteAddr->sin_addr, buffer.data(), buffer.size());

	std::string log{ std::format("[hh:mm:ss] {} Connected", buffer.c_str()) };
	if (std::unique_lock lock{ m_logMutex })
		m_logs.push_back(log);

	/*
	// 해당 소켓을 IOCP에 등록
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_clientSocket), m_hIOCP, m_sessionID, 0);

	// 클라이언트 세션 추가
	auto& session{ m_sessions[m_sessionID] };
	session.id = m_sessionID;
	session.socket = m_clientSocket;
	session.overlappedEx.op = IOOperation::RECV;
	++m_sessionID;

	WSABUF wsaBuf{ Session::BUFFER_SIZE, session.recvBuffer.get() };
	DWORD flag{};
	if (::WSARecv(session.socket, &wsaBuf, 1, 0, &flag, &session.overlappedEx, NULL))
	{
		if (::WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "ACCEPT FAIL - WSARecv");
			return;
		}
	}
	*/
}

/*
void Acceptor::OnReceive(size_t sessionID, Packet::size_type ioSize)
{
	if (!m_sessions.contains(sessionID))
		return;

	auto& session{ m_sessions.at(sessionID) };
	do
	{
		// 조립 중인 패킷이 있으면 패킷 뒤에 데이터 추가
		if (session.remainSize > 0)
		{
			session.packet->EncodeBuffer(session.recvBuffer.get(), ioSize);
			session.remainSize -= ioSize;
			break;
		}

		// 패킷 크기가 수신한 크기 보다 크면 아직 받을 데이터가 남은 것
		Packet::size_type size{ 0 };
		std::memcpy(&size, session.recvBuffer.get(), sizeof(size));
		if (size > ioSize)
			session.remainSize = size - ioSize;

		// 조립 시작
		session.packet = std::make_shared<Packet>(session.recvBuffer.get(), ioSize);
	} while (false);

	// 완성
	if (session.remainSize == 0)
	{
		session.packet->SetOffset(0);
		App::OnPacket->Notify(sessionID, session.packet);
	}

	WSABUF wsaBuf{ Session::BUFFER_SIZE, session.recvBuffer.get() };
	DWORD flag{};
	::WSARecv(session.socket, &wsaBuf, 1, 0, &flag, &session.overlappedEx, NULL);
}

void Acceptor::OnDisconnect(size_t sessionID)
{
	if (!m_sessions.contains(sessionID))
		return;

	auto& clientSocket{ m_sessions.at(sessionID) };
	::closesocket(clientSocket.socket);
	m_sessions.erase(sessionID);

	if (std::unique_lock lock{ m_logMutex })
		m_logs.push_back("[hh:mm:ss] Disconnect");
}

void Acceptor::OnPacket(size_t sessionID, const std::shared_ptr<Packet>& packet)
{
	switch (packet->GetType())
	{
	case Packet::Type::CLIENT_TryLogin:
	{
		std::vector<int> v;
		for (int i = 0; i < 1000; ++i)
			v.push_back(packet->Decode<int>());

		Packet packet{ Packet::Type::LOGIN_TryLogin };
		packet.Encode(std::string{ "HELLO, PACKET" });
		packet.End();
		::send(m_sessions[sessionID].socket, packet.GetBuffer(), packet.GetSize(), 0);
		break;
	}
	default:
		break;
	}
}
*/