#include "Stdafx.h"
#include "App.h"
#include "LoginServer.h"
#include "ServerManager.h"

ServerManager::ServerManager() :
	m_iocp{ INVALID_HANDLE_VALUE }
{
	m_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_iocp == INVALID_HANDLE_VALUE)
	{
		assert(false && "CREATE IOCP HANDLE FAIL");
		return;
	}

	// 로그인 서버에 연결
	if (!Connect(IServer::Type::Login, L"127.0.0.1", 9000))
		return;

	m_thread = std::jthread{ std::bind_front(&ServerManager::Run, this) };
}

ServerManager::~ServerManager()
{
	m_thread.request_stop();
	::CloseHandle(m_iocp);
}

bool ServerManager::Connect(IServer::Type type, std::wstring_view ip, unsigned short port)
{
	std::unique_ptr<IServer> server;
	switch (type)
	{
	case IServer::Type::Login:
		server = std::make_unique<LoginServer>();
		break;
	case IServer::Type::Game:
		break;
	case IServer::Type::Chat:
		break;
	default:
		return false;
	}
	if (!server->Connect(ip, port, m_iocp))
	{
		assert(false && "CONNECT TO SERVER FAIL");
		return false;
	}
	m_servers.push_back(std::move(server));
	return true;
}

void ServerManager::Disconnect(IServer::Type type)
{
	std::erase_if(m_servers, [type](auto& server) { return server->GetType() == type; });
}

bool ServerManager::IsConnected(IServer::Type type)
{
	return std::ranges::find_if(m_servers, [type](auto& server) { return server->GetType() == type; }) != m_servers.end();
}

void ServerManager::SendPacket(IServer::Type type, const Packet& packet)
{
	auto it{ std::ranges::find_if(m_servers, [type](auto& server) { return server->GetType() == type; }) };
	if (it != m_servers.end())
		(*it)->SendPacket(packet);
}

void ServerManager::Run(std::stop_token stoken)
{
	unsigned long ioSize{};
	IServer* server{};
	OVERLAPPEDEX* overlappedEx{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_iocp, &ioSize, reinterpret_cast<PULONG_PTR>(&server), reinterpret_cast<OVERLAPPED**>(&overlappedEx), INFINITE))
		{
			if (ioSize > 0)
				OnReceive(server, static_cast<Packet::Size>(ioSize));
			else
				OnDisconnect(server);
			break;
		}

		int error{ ::WSAGetLastError() };
		switch (error)
		{
		case ERROR_NETNAME_DELETED: // 서버에서 강제로 연결 끊음
			OnDisconnect(server);
			continue;
		case ERROR_ABANDONED_WAIT_0: // IOCP 핸들 닫힘
			continue;
		default:
			assert(false && "IOCP ERROR");
			continue;
		}
	}
}

void ServerManager::OnReceive(IServer* server, Packet::Size ioSize)
{
	auto& socket{ server->GetSocket() };
	if (socket.packet && socket.remainSize > 0)
	{
		socket.packet->EncodeBuffer(socket.buffer.data(), ioSize);
		socket.remainSize -= ioSize;
	}
	else
	{
		socket.packet = std::make_unique<Packet>(socket.buffer.data(), ioSize);

		Packet::Size size{ 0 };
		std::memcpy(&size, socket.buffer.data(), sizeof(size));
		if (size > ioSize)
			socket.remainSize = size - ioSize;
	}

	if (socket.packet && socket.remainSize == 0)
	{
		socket.packet->SetOffset(0);
		App::OnPacket.Notify(*socket.packet);
		socket.packet.reset();
	}

	WSABUF wsaBuf{ static_cast<unsigned long>(socket.buffer.size()), socket.buffer.data() };
	DWORD flag{};
	::WSARecv(socket.socket, &wsaBuf, 1, 0, &flag, &socket.overlappedEx, nullptr);
}

void ServerManager::OnDisconnect(IServer* server)
{
	Disconnect(server->GetType());
}