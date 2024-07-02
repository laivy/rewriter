#include "Stdafx.h"
#include "App.h"
#include "ServerManager.h"

ServerManager::ServerManager() :
	m_hIOCP{ INVALID_HANDLE_VALUE }
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

	Connect(Server::Type::LOGIN, "127.0.0.1", 9000);
}

ServerManager::~ServerManager()
{
	::WSACleanup();
}

bool ServerManager::Connect(Server::Type type, std::string_view ip, unsigned short port)
{
	auto& server{ m_servers[type] };
	server.ip = ip;
	server.port = port;

	SOCKET s{ ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED) };
	if (s == INVALID_SOCKET)
	{
		assert(false && "CREATE SOCKET FAIL");
		return false;
	}

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = ::htons(port);
	::inet_pton(AF_INET, ip.data(), &(addr.sin_addr.s_addr));
	if (::WSAConnect(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), nullptr, nullptr, nullptr, nullptr))
	{
		assert(false && "CONNECT FAIL");
		return false;
	}

	if (!::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), m_hIOCP, reinterpret_cast<unsigned long long>(&server), 0))
	{
		assert(false && "REGISTER IOCP FAIL");
		return false;
	}

	auto& socket{ server.socket };
	socket.socket = s;
	socket.overlappedEx.op = OVERLAPPEDEX::IOOP::RECEIVE;

	WSABUF wsaBuf{ static_cast<unsigned long>(socket.buffer.size()), socket.buffer.data() };
	DWORD flag{};
	::WSARecv(socket.socket, &wsaBuf, 1, 0, &flag, &socket.overlappedEx, nullptr);
	return true;
}

void ServerManager::Disconnect(Server::Type type)
{
	if (!m_servers.contains(type))
		return;
	::shutdown(m_servers[type].socket.socket, SD_BOTH);
	::closesocket(m_servers[type].socket.socket);
	m_servers.erase(type);
}

bool ServerManager::IsConnected(Server::Type type)
{
	if (!m_servers.contains(type))
		return false;
	if (m_servers[type].socket.socket == INVALID_SOCKET)
		return false;
	return true;
}

void ServerManager::SendPacket(Server::Type type, const Packet& packet)
{
	if (IsConnected(type))
		::send(m_servers[type].socket.socket, packet.GetBuffer(), packet.GetSize(), 0);
}

void ServerManager::Run(std::stop_token stoken)
{
	unsigned long ioSize{};
	size_t serverType{};
	OVERLAPPEDEX* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_hIOCP, &ioSize, reinterpret_cast<PULONG_PTR>(&serverType), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
		{
			switch (overlappedEx->op)
			{
			case OVERLAPPEDEX::IOOP::RECEIVE:
			{
				if (ioSize > 0)
					OnReceive(static_cast<Server::Type>(serverType), static_cast<Packet::size_type>(ioSize));
				else
					OnDisconnect(static_cast<Server::Type>(serverType));
				break;
			}
			default:
				assert(false);
				break;
			}
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 서버에서 강제로 연결 끊음
			OnDisconnect(static_cast<Server::Type>(serverType));
			continue;
		case ERROR_ABANDONED_WAIT_0: // IOCP 핸들 닫힘
			continue;
		default:
			assert(false && "IOCP ERROR");
			continue;
		}
	}
}

void ServerManager::OnReceive(Server::Type type, Packet::size_type ioSize)
{
	auto& socket{ m_servers.at(type).socket };
	if (socket.packet && socket.remainSize > 0)
	{
		socket.packet->EncodeBuffer(socket.buffer.data(), ioSize);
		socket.remainSize -= ioSize;
	}
	else
	{
		socket.packet = std::make_unique<Packet>(socket.buffer.data(), ioSize);

		Packet::size_type size{ 0 };
		std::memcpy(&size, socket.buffer.data(), sizeof(size));
		if (size > ioSize)
			socket.remainSize = size - ioSize;
	}

	if (socket.packet && socket.remainSize == 0)
	{
		socket.packet->SetOffset(0);
		App::OnPacket->Notify(*socket.packet);
		socket.packet.reset();
	}

	WSABUF wsaBuf{ static_cast<unsigned long>(socket.buffer.size()), socket.buffer.data() };
	DWORD flag{};
	::WSARecv(socket.socket, &wsaBuf, 1, 0, &flag, &socket.overlappedEx, nullptr);
}

void ServerManager::OnDisconnect(Server::Type type)
{
	auto& server{ m_servers.at(type) };
	if (server.socket.socket != INVALID_SOCKET)
	{
		::shutdown(server.socket.socket, SD_BOTH);
		::closesocket(server.socket.socket);
	}
	m_servers.erase(type);
}