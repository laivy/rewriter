#include "Stdafx.h"
#include "ClientAcceptor.h"
#include "User.h"

ClientAcceptor::ClientAcceptor() :
	m_isActive{ false },
	m_acceptContext{},
	m_sessionId{}
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
	m_acceptContext.listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_acceptContext.listenSocket == INVALID_SOCKET)
	{
		assert(false && "FAIL WSASocket - listenSocket");
		return;
	}

	SOCKADDR_IN serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = ::htons(9000);
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	if (::bind(m_acceptContext.listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		assert(false && "FAIL bind");
		return;
	}

	BOOL on{ TRUE };
	if (::setsockopt(m_acceptContext.listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<char*>(&on), sizeof(on)) == SOCKET_ERROR)
	{
		assert(false && "FAIL setsockopt");
		return;
	}

	if (::listen(m_acceptContext.listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		assert(false && "FAIL listen");
		return;
	}

	m_acceptContext.hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_acceptContext.hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "FAIL CreateIoCompletionPort()");
		return;
	}

	if (!::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_acceptContext.listenSocket), m_acceptContext.hIOCP, 0, 0))
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

	m_acceptContext.clientSocket = clientSocket;
	if (!::AcceptEx(
		m_acceptContext.listenSocket,
		m_acceptContext.clientSocket,
		m_acceptContext.overlappedEx.acceptBuffer.data(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		nullptr,
		&m_acceptContext.overlappedEx
	))
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
	unsigned long ioSize{};
	size_t socketID{};
	OVERLAPPEDEX* overlappedEx{};

	while (m_isActive)
	{
		if (!::GetQueuedCompletionStatus(m_acceptContext.hIOCP, &ioSize, reinterpret_cast<PULONG_PTR>(&socketID), reinterpret_cast<OVERLAPPED**>(&overlappedEx), SOCKET_TIMEOUT_MILLISEC))
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
				OnReceive(socketID, ioSize);
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

	if (::setsockopt(m_acceptContext.clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&m_acceptContext.listenSocket), sizeof(m_acceptContext.listenSocket)))
	{
		assert(false && "ACCEPT FAIL - setsockopt");
		return;
	}

	// 해당 소켓을 IOCP에 등록
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_acceptContext.clientSocket), m_acceptContext.hIOCP, m_sessionId, 0);

	// ClientSocket 추가
	// 해당 소켓을 수신 상태로 변경
	if (std::unique_lock lock{ m_socketMutex })
	{
		auto& session{ m_sessions[m_sessionId] };
		session.id = m_sessionId;
		session.socket = m_acceptContext.clientSocket;
		session.recvOverlappedEx.op = IOOperation::RECV;
		++m_sessionId;

		WSABUF wsaBuf{ OVERLAPPEDEX::BUFFER_SIZE, session.recvOverlappedEx.recvBuffer.get() };
		DWORD flag{};
		if (::WSARecv(session.socket, &wsaBuf, 1, 0, &flag, &session.recvOverlappedEx, NULL))
		{
			if (::WSAGetLastError() != ERROR_IO_PENDING)
			{
				assert(false && "ACCEPT FAIL - WSARecv");
				return;
			}
		}
	}

	// 새로운 Accept 소켓 생성
	m_acceptContext.overlappedEx = {};
	m_acceptContext.clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	::AcceptEx(
		m_acceptContext.listenSocket,
		m_acceptContext.clientSocket,
		m_acceptContext.overlappedEx.acceptBuffer.data(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		nullptr,
		&m_acceptContext.overlappedEx
	);
}

void ClientAcceptor::OnReceive(size_t socketID, unsigned long ioSize)
{
	std::lock_guard lock{ m_socketMutex };
	if (!m_sessions.contains(socketID))
		return;

	auto& session{ m_sessions.at(socketID) };
	do
	{
		// 조립 중인 패킷이 있으면 패킷 뒤에 데이터 추가
		if (session.remainSize > 0)
		{
			session.packet->EncodeBuffer(session.recvOverlappedEx.recvBuffer.get(), ioSize);
			session.remainSize -= ioSize;
			break;
		}

		// 패킷 크기가 수신한 크기 보다 크면 아직 받을 데이터가 남은 것
		unsigned int size{ 0 };
		std::memcpy(&size, session.recvOverlappedEx.recvBuffer.get(), sizeof(size));
		if (size > ioSize)
			session.remainSize = size - ioSize;

		// 조립 시작
		session.packet = std::make_unique<Packet>(session.recvOverlappedEx.recvBuffer.get(), ioSize);
	} while (false);

	// 완성
	if (session.remainSize == 0)
	{
		session.packet->SetOffset(0);
		
		std::wstring wstr{ std::format(L"{} : ", socketID) };
		for (int i = 0; i < 1000; ++i)
			wstr += std::to_wstring(i) + L",";
		wstr += L"\n";
		::OutputDebugString(wstr.c_str());
	}

	WSABUF wsaBuf{ OVERLAPPEDEX::BUFFER_SIZE, session.recvOverlappedEx.recvBuffer.get() };
	DWORD flag{};
	::WSARecv(session.socket, &wsaBuf, 1, 0, &flag, &session.recvOverlappedEx, NULL);
}

void ClientAcceptor::OnDisconnect(size_t socketID)
{
	std::lock_guard lock{ m_socketMutex };
	if (!m_sessions.contains(socketID))
		return;

	auto& clientSocket{ m_sessions.at(socketID) };
	::closesocket(clientSocket.socket);
	m_sessions.erase(socketID);

	if (std::unique_lock lock{ m_logMutex })
		m_logs.push_back("[hh:mm:ss] Disconnect");
}
