#pragma once

enum class ServerType : std::uint8_t
{
	LOGIN,
	GAME,
	CHAT,
	COUNT
};

class ServerThread :
	public std::jthread,
	public TSingleton<ServerThread>
{
private:
	enum class IOOperation
	{
		CONNECT, RECV
	};

	struct OVERLAPPEDEX : public OVERLAPPED
	{
		IOOperation op{ IOOperation::CONNECT };
	};

	struct Server
	{
		std::mutex mutex;

		std::string ip;
		unsigned short port{};
		SOCKET socket{ INVALID_SOCKET };
		OVERLAPPEDEX overlappedEx{};
		std::array<char, 512> buffer{};

		Packet::size_type remainSize{ 0 };
		std::shared_ptr<Packet> packet;
	};

public:
	ServerThread();
	~ServerThread();

	void SendPacket(ServerType type, const Packet& packet);

	std::shared_ptr<Packet> PopPacket();

private:
	void Run(std::stop_token stoken);

	void OnReceive(ServerType type, Packet::size_type ioSize);
	void OnDisconnect(ServerType type);

	void Connect(ServerType type, std::string_view ip, unsigned short port);

private:
	HANDLE m_hIOCP;
	std::array<Server, static_cast<size_t>(ServerType::COUNT)> m_servers;

	std::mutex m_packetMutex;
	std::queue<std::shared_ptr<Packet>> m_packets;
};