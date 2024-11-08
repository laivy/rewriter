#pragma once
#include "Socket.h"

class ServerSocket : public ISocket
{
public:
	enum class Type
	{
		None,
		Login,
		Center
	};

public:
	ServerSocket(SOCKET socket = INVALID_SOCKET);
	virtual ~ServerSocket();

	virtual void OnConnect();

	Type GetType() const;

protected:
	void Connect(const std::wstring& ip, unsigned short port);

protected:
	Type m_type;

private:
	std::jthread m_connectThread;
};
