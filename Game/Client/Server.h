#pragma once
#include "Common/Socket.h"

class IServer abstract
{
public:
	enum class Type
	{
		Login,
		Game,
		Chat
	};

public:
	IServer(Type type);
	virtual ~IServer();

	bool Connect(std::wstring_view ip, unsigned short port, HANDLE iocp);
	void SendPacket(const Packet& packet) const;

	Type GetType() const;
	SocketEx& GetSocket();

private:
	Type m_type;
	SocketEx m_socket;
};