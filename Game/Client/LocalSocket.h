#pragma once
#include "Common/Socket.h"

class LocalSocket : public ISocket
{
public:
	LocalSocket();
	~LocalSocket();

	virtual void OnDisconnect() override final;
	virtual void OnSend(Packet::Size size) override final;
	virtual void OnReceive(Packet::Size size) override final;

	bool Connect(std::wstring_view ip, unsigned short port);
	void Disconnect();
	void Send(Packet& packet);
	void Receive();

	bool IsConnected() const;

private:
	SOCKET m_socket;
	std::vector<SendBuffer> m_sendBuffers;
	ReceiveBuffer m_receiveBuffer;
};