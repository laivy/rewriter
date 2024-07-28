#pragma once

class User;

struct OVERLAPPEDEX : OVERLAPPED
{
	enum class IOType
	{
		Accept,
		Send,
		Receive
	};

	IOType ioType;
};

class ClientSocket
{
public:
	ClientSocket(SOCKET socket);
	~ClientSocket();

	void OnReceive(Packet::Size transferredSize);
	void OnDisconnect();

	void SetReceive();

private:
	void OnPacket(Packet& packet);
	void OnRegisterRequest(Packet& packet);
	void OnLoginRequest(Packet& packet);

	void Send(const Packet& packet) const;

private:
	SOCKET m_socket;
	OVERLAPPEDEX m_overlappedEx;
	std::array<char, 512> m_recvBuffer;

	std::unique_ptr<Packet> m_packet;
	Packet::Size m_packetRemainSize;
		
	std::shared_ptr<User> m_user;
};