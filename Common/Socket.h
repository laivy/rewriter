#pragma once
#include <WinSock2.h>
#include "Packet.h"

class ISocket abstract
{
public:
	enum class IOOperation : uint8_t
	{
		Connect,
		Accept,
		Send,
		Receive
	};

	struct OverlappedEx : OVERLAPPED
	{
		IOOperation op{ IOOperation::Accept };
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
		std::unique_ptr<char[]> buffer;
		Packet::Size size;
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
	SOCKET Detach();

	bool IsConnected() const;
	std::string GetIP() const;

protected:
	std::recursive_mutex m_mutex;
	SOCKET m_socket;

private:
	std::string m_ip;
	std::list<SendBuffer> m_sendBuffers;
	ReceiveBuffer m_receiveBuffer;
};
