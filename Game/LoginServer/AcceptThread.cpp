#include "Stdafx.h"
#include "AcceptThread.h"
#include "App.h"
#include "User.h"

AcceptThread::AcceptThread() :
	m_isActive{ false },
	m_context{},
	m_sessionID{},
	std::jthread{ &AcceptThread::Run, this }
{
	App::OnPacket->Register(std::bind_front(&AcceptThread::OnPacket, this));
}

AcceptThread::~AcceptThread()
{
	m_isActive = false;
}

void AcceptThread::Render()
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

void AcceptThread::Run()
{
	m_context.listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_context.listenSocket == INVALID_SOCKET)
	{
		assert(false && "CREATE LISTEN SOCKET FAIL");
		return;
	}

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(9000);
	addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	if (::bind(m_context.listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
	{
		assert(false && "LISTEN SOCKET BIND FAIL");
		return;
	}

	BOOL option{ TRUE };
	if (::setsockopt(m_context.listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<char*>(&option), sizeof(option)) == SOCKET_ERROR)
	{
		assert(false && "LISTEN SOCKET SETSOCKOPT FAIL");
		return;
	}

	if (::listen(m_context.listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		assert(false && "LISTEN SOCKET LISTEN FAIL");
		return;
	}

	m_context.hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_context.hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "CREATE IOCP FAIL");
		return;
	}

	if (!::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_context.listenSocket), m_context.hIOCP, 0, 0))
	{
		assert(false && "REGISTER IOCP FAIL");
		return;
	}

	m_context.clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (m_context.clientSocket == INVALID_SOCKET)
	{
		assert(false && "CREATE CLIENT SOCKET FAIL");
		return;
	}

	if (!::AcceptEx(
		m_context.listenSocket,
		m_context.clientSocket,
		m_context.overlappedEx.acceptBuffer.data(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		nullptr,
		&m_context.overlappedEx
	))
	{
		if (::WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "ACCEPT FAIL");
			return;
		}
	}
	m_isActive = true;

	unsigned long ioSize{};
	size_t sessionID{};
	OVERLAPPEDEX* overlappedEx{};
	while (m_isActive)
	{
		if (!::GetQueuedCompletionStatus(m_context.hIOCP, &ioSize, reinterpret_cast<PULONG_PTR>(&sessionID), reinterpret_cast<OVERLAPPED**>(&overlappedEx), SOCKET_TIMEOUT_MILLISEC))
		{
			int error{ ::WSAGetLastError() };
			switch (error)
			{
			case ERROR_NETNAME_DELETED: // 클라이언트에서 강제로 연결 끊음
				OnDisconnect(sessionID);
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
				OnReceive(sessionID, static_cast<Packet::size_type>(ioSize));
			else
				OnDisconnect(sessionID);
			break;
		}
		default:
			assert(false && "INVALID IO OPERATION");
			break;
		}
	}
}

void AcceptThread::OnAccept(OVERLAPPEDEX* overlappedEx)
{
	// 연결된 클라이언트 정보를 로그로 출력
	SOCKADDR_IN* localAddr{};
	SOCKADDR_IN* remoteAddr{};
	int localAddrSize{};
	int remoteAddrSize{};

	::GetAcceptExSockaddrs(
		&overlappedEx->acceptBuffer,
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

	if (::setsockopt(m_context.clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_context.listenSocket), sizeof(m_context.listenSocket)))
	{
		assert(false && "ACCEPT FAIL - setsockopt");
		return;
	}

	// 해당 소켓을 IOCP에 등록
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_context.clientSocket), m_context.hIOCP, m_sessionID, 0);

	// 클라이언트 세션 추가
	auto& session{ m_sessions[m_sessionID] };
	session.id = m_sessionID;
	session.socket = m_context.clientSocket;
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

	// 새로운 Accept 소켓 생성
	m_context.overlappedEx = {};
	m_context.clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	::AcceptEx(
		m_context.listenSocket,
		m_context.clientSocket,
		m_context.overlappedEx.acceptBuffer.data(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		nullptr,
		&m_context.overlappedEx
	);
}

void AcceptThread::OnReceive(size_t sessionID, Packet::size_type ioSize)
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

void AcceptThread::OnDisconnect(size_t sessionID)
{
	if (!m_sessions.contains(sessionID))
		return;

	auto& clientSocket{ m_sessions.at(sessionID) };
	::closesocket(clientSocket.socket);
	m_sessions.erase(sessionID);

	if (std::unique_lock lock{ m_logMutex })
		m_logs.push_back("[hh:mm:ss] Disconnect");
}

void AcceptThread::OnPacket(size_t sessionID, const std::shared_ptr<Packet>& packet)
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
