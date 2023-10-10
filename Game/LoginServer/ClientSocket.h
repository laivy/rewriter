#pragma once

class ClientSocket
{
public:
	friend class IOCPThread;

	ClientSocket(int socketID, SOCKET socket);
	~ClientSocket();

private:
	int m_socketID;
	SOCKET m_socket;
	OVERLAPPEDEX m_overlappedEx;
};