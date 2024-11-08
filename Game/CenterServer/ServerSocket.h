#pragma once
#include "Common/Socket.h"

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
	virtual ~ServerSocket() = default;

	virtual void OnComplete(Packet& packet) override;

	Type GetType() const;

private:
	Type m_type;
};
