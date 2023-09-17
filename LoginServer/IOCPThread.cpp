#include "Stdafx.h"
#include "User.h"
#include "IOCPThread.h"

IOCPThread::IOCPThread() :
	std::thread{ &IOCPThread::Run, this },
	m_isActive{ true },
	m_isAcceptable{ false },
	m_hIOCP{ INVALID_HANDLE_VALUE },
	m_overlappedEx{},
	m_listenSocket{ INVALID_SOCKET }
{
	detach();
}

IOCPThread::~IOCPThread()
{
	m_isActive = false;
	if (std::thread::joinable())
		std::thread::join();
}

void IOCPThread::Render()
{
	ImGui::Begin(CW2A{ TEXT("유저"), CP_UTF8});
	if (ImGui::BeginTabBar("Tab"))
	{
		if (ImGui::BeginTabItem(CW2A{ TEXT("정보"), CP_UTF8 }))
		{
			ImGui::Text(CW2A{ TEXT("소켓 상태"), CP_UTF8 });
			ImGui::SameLine(100);
			ImGui::Text(":");
			ImGui::SameLine();

			std::wstring str{};
			ImVec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
			if (m_isAcceptable)
			{
				str = TEXT("가능");
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				str = TEXT("불가능");
				color = { 1.0f, 0.0f, 0.0f, 1.0f };
			}
			ImGui::TextColored(color, CW2A{ str.c_str(), CP_UTF8 });

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(CW2A{ TEXT("로그"), CP_UTF8 }))
		{
			if (ImGui::BeginChild("LogView"))
			{
				std::unique_lock lock{ m_logsMutex };
				for (const auto& log : m_logs)
				{
					ImGui::Text(CW2A{ log.c_str(), CP_UTF8 });
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

void IOCPThread::Run()
{
	WSADATA wsaData{};
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		assert(false && "FAIL WSAStartup()");
		return;
	}

	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		assert(false && "FAIL WSASocket() - listenSocket");
		return;
	}

	SOCKADDR_IN serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9000);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(m_listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		assert(false && "FAIL bind()");
		return;
	}

	BOOL on{ TRUE };
	if (setsockopt(m_listenSocket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, reinterpret_cast<char*>(std::addressof(on)), sizeof(on)) == SOCKET_ERROR)
	{
		assert(false && "FAIL setsockopt()");
		return;
	}

	if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		assert(false && "FAIL listen()");
		return;
	}

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "FAIL CreateIoCompletionPort()");
		return;
	}

	if (!CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), m_hIOCP, 0, 0))
	{
		assert(false && "FAIL CreateIoCompletionPort()");
		return;
	}

	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET)
	{
		assert(false && "FAIL WSASocket() - acceptSocket");
		return;
	}

	m_overlappedEx = {};
	m_overlappedEx.op = IOOperation::ACCEPT;
	m_overlappedEx.socket = clientSocket;
	if (!AcceptEx(m_listenSocket, m_overlappedEx.socket, m_overlappedEx.buffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, NULL, &m_overlappedEx))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			assert(false && "FAIL AcceptEx()");
			return;
		}
	}

	m_isAcceptable = true;

	// 워커쓰레드 생성
	SYSTEM_INFO si{};
	GetSystemInfo(&si);
	for (UINT i = 0; i < si.dwNumberOfProcessors * 2; ++i)
		m_workerThreads.emplace_back(&IOCPThread::Work, this);

	for (auto& t : m_workerThreads)
		if (t.joinable())
			t.join();
}

void IOCPThread::Work()
{
	while (true)
	{
		DWORD ioSize{};
		LONG64 iocpKey{};
		OVERLAPPED* overlapped{};

		if (!GetQueuedCompletionStatus(m_hIOCP, &ioSize, reinterpret_cast<PULONG_PTR>(&iocpKey), &overlapped, INFINITE))
		{
			assert(false && "FAIL GetQueuedCompletionStatus()");
			return;
		}

		OVERLAPPEDEX* overlappedEx{ reinterpret_cast<OVERLAPPEDEX*>(overlapped) };
		switch (overlappedEx->op)
		{
		case IOOperation::ACCEPT:
		{
			OnAccept(overlappedEx);
			break;
		}
		}
	}
}

void IOCPThread::OnAccept(OVERLAPPEDEX* overlappedEx)
{
	// 연결된 클라이언트 정보를 로그로 출력
	SOCKADDR_IN* localAddr{};
	SOCKADDR_IN* remoteAddr{};
	int localAddrSize{};
	int remoteAddrSize{};

	GetAcceptExSockaddrs(
		overlappedEx->buffer,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<SOCKADDR**>(&localAddr),
		&localAddrSize,
		reinterpret_cast<SOCKADDR**>(&remoteAddr),
		&remoteAddrSize
	);

	std::string buffer[2]{};
	buffer[0].resize(16);
	buffer[1].resize(16);
	inet_ntop(AF_INET, &localAddr->sin_addr, buffer[0].data(), sizeof(buffer[0]));
	inet_ntop(AF_INET, &remoteAddr->sin_addr, buffer[1].data(), sizeof(buffer[1]));

	std::wstring log{ TEXT("[알림] ") };
	log += CA2W{ buffer[1].c_str(), CP_UTF8 };
	log += TEXT(" 클라이언트가 접속했습니다.");

	if (std::unique_lock lock{ m_logsMutex })
		m_logs.push_back(log);

	/*
	* TODO 유저 객체 생성
	*/

	// 새로운 연결을 위한 소켓 생성
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(overlappedEx->socket), m_hIOCP, 0, 0);
	overlappedEx->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	AcceptEx(
		m_listenSocket,
		overlappedEx->socket,
		overlappedEx->buffer,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		NULL,
		overlappedEx
	);
}
