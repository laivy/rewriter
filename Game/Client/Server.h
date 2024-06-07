#pragma once

class IServer;

extern std::map<int, std::unique_ptr<IServer>> g_servers;

template<class T>
requires (std::is_base_of_v<IServer, T> && !std::is_same_v<IServer, T>)
void Connect()
{
	if (g_servers.contains(T::SERVER_INDEX))
		return;

	g_servers[T::SERVER_INDEX] = std::make_unique<T>();
	g_servers[T::SERVER_INDEX]->Connect();
}

template<class T>
requires (std::is_base_of_v<IServer, T> && !std::is_same_v<IServer, T>)
void Disconnect()
{
	if (g_servers.contains(T::SERVER_INDEX))
		g_servers[T::SERVER_INDEX]->Disconnect();
}

template<class T>
requires (std::is_base_of_v<IServer, T> && !std::is_same_v<IServer, T>)
void Send(const Packet& packet)
{
	if (g_servers.contains(T::SERVER_INDEX))
		g_servers[T::SERVER_INDEX]->Send(packet);
}

class IServer abstract
{
public:
	IServer();
	virtual ~IServer();

	virtual void Connect() = 0;
	virtual void Disconnect();

	virtual void Send(const Packet& packet);

	bool IsConnected() const;

protected:
	void Connect(std::string_view ip, int port);

protected:
	SOCKET m_socket;
};

class LoginServer : public IServer
{
public:
	LoginServer() = default;
	~LoginServer() = default;

	virtual void Connect() override final;

public:
	static constexpr auto SERVER_INDEX{ 0 };

private:
	static constexpr auto IP{ "127.0.0.1" };
	static constexpr auto PORT{ 9000 };
};
