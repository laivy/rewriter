#pragma once

class CenterServer : public TSingleton<CenterServer>
{
public:
	CenterServer(std::wstring_view config);
	~CenterServer();

	void Send(Packet& packet) const;

	bool IsConnected() const;

private:
	void Run(std::stop_token stoken);
	void OnReceive(int received);
	void OnDisconnect();
	void OnPacket(Packet& packet);

private:
	std::jthread m_thread;

	SOCKET m_socket;
	std::wstring m_ip;
	unsigned short m_port;
	std::array<char, 512> m_buffer;

	std::unique_ptr<Packet> m_packet;
	Packet::Size m_remainSize;
};