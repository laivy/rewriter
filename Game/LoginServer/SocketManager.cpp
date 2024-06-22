#include "Stdafx.h"
#include "SocketManager.h"
#include "User.h"
#include "UserManager.h"

SocketManager::SocketManager()
{
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == INVALID_HANDLE_VALUE)
	{
		assert(false && "SOCKET MANAGER CREATE IOCP FAIL");
		return;
	}

	for (auto& thread : m_threads)
		thread = std::jthread{ std::bind_front(&SocketManager::Run, this) };
}

void SocketManager::Register(const std::shared_ptr<User>& user, const std::shared_ptr<Socket>& socket)
{
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket->socket), m_hIOCP, reinterpret_cast<unsigned long long>(user.get()), 0);
	
	WSABUF wsaBuf{ static_cast<unsigned long>(socket->buffer.size()), socket->buffer.data() };
	DWORD flag{};
	::WSARecv(socket->socket, &wsaBuf, 1, 0, &flag, &socket->overlapped, nullptr);
}

void SocketManager::Run(std::stop_token stoken)
{
	unsigned long ioSize{};
	User* user{ nullptr };
	OVERLAPPED* overlapped{};
	while (!stoken.stop_requested())
	{
		if (::GetQueuedCompletionStatus(m_hIOCP, &ioSize, reinterpret_cast<unsigned long long*>(&user), &overlapped, 1000))
		{
			if (ioSize > 0)
				OnReceive(user, ioSize);
			else
				OnDisconnect(user);
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
		Packet::size_type size{ 0 };
		std::memcpy(&size, socket->buffer.data(), sizeof(size));
		if (size > ioSize)
			socket->remainSize = size - ioSize;

		socket->packet = std::make_unique<Packet>(socket->buffer.data(), static_cast<Packet::size_type>(ioSize));
	}

	if (socket->packet && socket->remainSize == 0)
	{
		socket->packet->SetOffset(0);
		user->OnPacket(*socket->packet);
		socket->packet.reset();
	}

	WSABUF wsaBuf{ static_cast<unsigned long>(socket->buffer.size()), socket->buffer.data() };
	DWORD flag{};
	::WSARecv(socket->socket, &wsaBuf, 1, 0, &flag, &socket->overlapped, nullptr);
}

void SocketManager::OnDisconnect(User* user)
{
	const auto& socket{ user->GetSocket() };
	::shutdown(socket->socket, SD_BOTH);
	::closesocket(socket->socket);

	if (auto um{ UserManager::GetInstance() })
		um->Unregister(user);
}