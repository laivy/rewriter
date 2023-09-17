#pragma once

class ClientSocket
{
public:
	ClientSocket(SOCKET socket);
	~ClientSocket() = default;

private:
	SOCKET m_socket;
};