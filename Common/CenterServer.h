#pragma once

class CenterServer : public TSingleton<CenterServer>
{
public:
	CenterServer(std::wstring_view config);
	~CenterServer() = default;

	bool IsConnected() const;

private:
	void Run(std::stop_token stoken);
	void OnReceive(int received);
	void OnDisconnect();

private:
	std::jthread m_thread;

	SOCKET m_socket;
	std::wstring m_ip;
	unsigned short m_port;
	std::array<char, 512> m_buffer;

	std::unique_ptr<Packet> m_packet;
	Packet::size_type m_remainSize;
};