#pragma once
#include "Packet.h"

class ISocket abstract
{
public:
	using ID = uint32_t;

	enum class Type
	{
		None,
		Client,
		Login,
		Game,
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
		Packet packet{ Protocol::Type::None };
	};

	struct ReceiveBuffer
	{
		OverlappedEx overlappedEx{};
		Packet packet{ Protocol::Type::None };
		std::array<char, 512> buffer{};
		int remainPacketSize{};
	};

public:
	ISocket(SOCKET socket = INVALID_SOCKET);
	virtual ~ISocket();

	operator SOCKET();

	virtual void OnConnect(bool success);
	virtual void OnDisconnect();
	virtual void OnSend(OverlappedEx* overlappedEx);
	virtual void OnReceive(Packet::Size ioSize);
	virtual void OnComplete(Packet& packet);

	bool Socket();
	bool Connect(std::wstring_view ip, unsigned short port);
	void Disconnect();
	void Send(Packet& packet);
	void Receive();
	void SetType(Type type);

	bool IsConnected() const;
	Type GetType() const;
	ID GetID() const;
	std::string GetIP() const;

private:
	static inline ID s_id{ 0 };

	std::recursive_mutex m_mutex;
	SOCKET m_socket;
	Type m_type;
	ID m_id;
	std::string m_ip;
	std::list<SendBuffer> m_sendBuffers;
	ReceiveBuffer m_receiveBuffer;
};
