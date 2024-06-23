#include "Stdafx.h"
#include "ServerThread.h"

ServerThread::ServerThread() :
	m_hIOCP{ INVALID_HANDLE_VALUE },
	m_servers{},
	std::jthread{ std::bind_front(&ServerThread::Run, this) }
{
}

ServerThread::~ServerThread()
{
	std::jthread::request_stop();
	std::jthread::join();
	for (auto& server : m_servers)
	{
		if (server.socket != INVALID_SOCKET)
			::closesocket(server.socket);
	}
	::WSACleanup();
}

void ServerThread::SendPacket(ServerType type, const Packet& packet)
{
	auto& server{ m_servers.at(static_cast<size_t>(type)) };

	std::lock_guard lock{ server.mutex };
	if (server.socket != INVALID_SOCKET)
		::send(server.socket, packet.GetBuffer(), packet.GetSize(), 0);
}

std::shared_ptr<Packet> ServerThread::PopPacket()
{
	std::lock_guard lock{ m_packetMutex };
	if (m_packets.empty())
		return nullptr;

	auto packet{ m_packets.front() };
	m_packets.pop();
	return packet;
}

void ServerThread::Run(std::stop_token stoken)
{
	WSADATA wsaData{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		assert(false && "WSA INIT FAIL");
		return;
	}

	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "CREATE IOCP HANDLE FAIL");
		return;
	}

	// 로그인 서버 연결
	Connect(ServerType::LOGIN, "127.0.0.1", 9000);

	unsigned long ioSize{};
	size_t serverType{};
	OVERLAPPEDEX* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (!::GetQueuedCompletionStatus(m_hIOCP, &ioSize, reinterpret_cast<PULONG_PTR>(&serverType), reinterpret_cast<OVERLAPPED**>(&overlappedEx), 1000))
		{
			int error{ ::WSAGetLastError() };
			switch (error)
			{
			case ERROR_NETNAME_DELETED: // 서버에서 강제로 연결 끊음
				OnDisconnect(static_cast<ServerType>(serverType));
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
		case IOOperation::CONNECT:
			break;
		case IOOperation::RECV:
		{
			if (ioSize > 0)
				OnReceive(static_cast<ServerType>(serverType), static_cast<Packet::size_type>(ioSize));
			else
				OnDisconnect(static_cast<ServerType>(serverType));
			break;
		}
		default:
			assert(false);
			break;
		}
	}
}

void ServerThread::OnReceive(ServerType type, Packet::size_type ioSize)
{
	auto& server{ m_servers.at(static_cast<size_t>(type)) };
	do
	{
		if (server.remainSize > 0)
		{
			server.packet->EncodeBuffer(server.buffer.data(), ioSize);
			server.remainSize -= ioSize;
			break;
		}

		Packet::size_type size{ 0 };
		std::memcpy(&size, server.buffer.data(), sizeof(size));
		if (size > ioSize)
			server.remainSize = size - ioSize;
		
		server.packet = std::make_shared<Packet>(server.buffer.data(), ioSize);
	} while (false);

	if (server.remainSize == 0)
	{
		server.packet->SetOffset(0);
		std::lock_guard lock{ m_packetMutex };
		m_packets.push(server.packet);
		server.packet.reset();
	}

	WSABUF wsaBuf{ static_cast<unsigned long>(server.buffer.size()), server.buffer.data() };
	DWORD flag{};
	::WSARecv(server.socket, &wsaBuf, 1, 0, &flag, &server.overlappedEx, nullptr);
}

void ServerThread::OnDisconnect(ServerType type)
{
	auto& server{ m_servers.at(static_cast<size_t>(type)) };
	std::lock_guard lock{ server.mutex };

	server.ip.clear();
	server.port = 0;
	if (server.socket != INVALID_SOCKET)
	{
		::shutdown(server.socket, SD_BOTH);
		::closesocket(server.socket);
		server.socket = INVALID_SOCKET;
	}
	server.overlappedEx = {};
	server.buffer.fill(0);
	server.remainSize = 0;
	server.packet.reset();
}

void ServerThread::Connect(ServerType type, std::string_view ip, unsigned short port)
{
	auto& server{ m_servers.at(static_cast<size_t>(type)) };
	server.ip = ip;
	server.port = port;
	server.socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	server.overlappedEx.op = IOOperation::RECV;

	auto completionKey{ static_cast<unsigned long>(type) };
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(server.socket), m_hIOCP, completionKey, 0);

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(port);
	::inet_pton(AF_INET, ip.data(), &(addr.sin_addr.s_addr));
	::WSAConnect(server.socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), nullptr, nullptr, nullptr, nullptr);

	WSABUF wsaBuf{ static_cast<unsigned long>(server.buffer.size()), server.buffer.data() };
	DWORD flag{};
	::WSARecv(server.socket, &wsaBuf, 1, 0, &flag, &server.overlappedEx, nullptr);
}