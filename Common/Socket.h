#pragma once
#include "Packet.h"

class ISocket abstract
{
public:
	enum class Type
	{
		None,
		Client,
		Login,
		Center
	};

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
		Packet packet{ Protocol::None };
	};

	struct ReceiveBuffer
	{
		OverlappedEx overlappedEx{};
		Packet packet{ Protocol::None };
		std::array<char, 512> buffer{};
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
	void SetType(Type type);

	bool IsConnected() const;
	std::string GetIP() const;
	Type GetType() const;

private:
	std::recursive_mutex m_mutex;
	Type m_type;
	SOCKET m_socket;
	std::string m_ip;
	std::list<SendBuffer> m_sendBuffers;
	ReceiveBuffer m_receiveBuffer;
};
