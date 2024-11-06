#pragma once
#include <WinSock2.h>
#include "Packet.h"

class ISocket abstract
{
public:
	enum class IOOperation : uint8_t
	{
		Connect,
		Send,
		Receive
	};

	struct OverlappedEx : OVERLAPPED
	{
		IOOperation op{ IOOperation::Connect };
	};

private:
	struct SendBuffer
	{
		SendBuffer();
		SendBuffer(const SendBuffer&) = delete;
		SendBuffer(SendBuffer&& other) noexcept;
		~SendBuffer() = default;
		SendBuffer& operator=(const SendBuffer&) = delete;
		SendBuffer& operator=(SendBuffer&& other) noexcept;

		OverlappedEx overlappedEx;
		Packet::Size size;
		std::unique_ptr<char[]> buffer;
	};

	struct ReceiveBuffer
	{
		OverlappedEx overlappedEx{};
		std::array<char, 1024> buffer{};
		std::unique_ptr<Packet> packet;
		int remainPacketSize{};
	};

public:
	ISocket(SOCKET socket = INVALID_SOCKET);
	virtual ~ISocket();

	operator SOCKET();

	virtual void OnSend(OverlappedEx* overlappedEx);
	virtual void OnReceive(Packet::Size ioSize);
	virtual void OnComplete(Packet& packet);
	virtual void OnDisconnect();

	bool Connect(std::wstring_view ip, unsigned short port);
	void Disconnect();
	void Send(Packet& packet);
	void Receive();

	bool IsConnected() const;

private:
	std::recursive_mutex m_mutex;
	SOCKET m_socket;
	std::list<SendBuffer> m_sendBuffers;
	ReceiveBuffer m_receiveBuffer;
};
