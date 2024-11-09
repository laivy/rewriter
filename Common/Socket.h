#pragma once
#include "Packet.h"

class ISocket abstract
{
public:
	enum class IOOperation : uint8_t
	{
		None,
		Connect,
		Accept,
		Send,
		Receive
	};

	struct OverlappedEx : OVERLAPPED
	{
		IOOperation op{ IOOperation::None };
	};

private:
	struct SendBuffer
	{
		OverlappedEx overlappedEx{};
		Packet packet{ Packet::Type::None };
	};

	struct ReceiveBuffer
	{
		OverlappedEx overlappedEx{};
		std::array<char, 1024> buffer{};
		std::unique_ptr<Packet> packet;
		int remainPacketSize{};
	};

public:
	ISocket();
	ISocket(SOCKET socket);
	virtual ~ISocket();

	operator SOCKET();

	virtual void OnConnect(bool success);
	virtual void OnDisconnect();
	virtual void OnSend(OverlappedEx* overlappedEx);
	virtual void OnReceive(Packet::Size ioSize);
	virtual void OnComplete(Packet& packet);

	bool Connect(std::wstring_view ip, unsigned short port);
	void Disconnect();
	void Send(Packet& packet);
	void Receive();

	bool IsConnected() const;
	std::string GetIP() const;

private:
	std::recursive_mutex m_mutex;
	SOCKET m_socket;
	std::string m_ip;
	std::list<SendBuffer> m_sendBuffers;
	ReceiveBuffer m_receiveBuffer;
};
