#pragma once
#include <WinSock2.h>
#include "Packet.h"

class ISocket abstract
{
public:
	enum class IOOperation : unsigned char
	{
		Connect,
		Send,
		Receive
	};

	struct OverlappedEx : OVERLAPPED
	{
		IOOperation op{ IOOperation::Connect };
	};

protected:
	struct SendBuffer
	{
		OverlappedEx overlappedEx;
		std::unique_ptr<char[]> buffer;
		Packet::Size size;

		SendBuffer();
		SendBuffer(const SendBuffer&) = delete;
		SendBuffer(SendBuffer&& other) noexcept;
		~SendBuffer() = default;
		SendBuffer& operator=(const SendBuffer&) = delete;
		SendBuffer& operator=(SendBuffer&& other) noexcept;
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
	virtual ~ISocket();

	operator SOCKET();

	virtual void OnDisconnect();
	virtual void OnSend(Packet::Size ioSize);
	virtual void OnReceive(Packet::Size ioSize);

	bool Connect(std::wstring_view ip, unsigned short port);
	void Disconnect();
	void Send(Packet& packet);
	void Receive();

	bool IsConnected() const;
	
protected:
	SOCKET m_socket;

private:
	std::vector<SendBuffer> m_sendBuffers;
	ReceiveBuffer m_receiveBuffer;
};