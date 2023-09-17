#pragma once

class IRequest;

class DBThread : 
	public std::thread,
	public TSingleton<DBThread>
{
public:
	DBThread();
	~DBThread();

	void Render();

private:
	void Init();
	void Run();

private:
	bool m_isActive;
	bool m_isConnected;

	// 연결 정보
	std::wstring m_ipport;
	std::wstring m_dbname;
	std::wstring m_username;
	std::wstring m_password;

	// 로그
	std::mutex m_logsMutex;
	std::vector<std::wstring> m_logs;

	// 요청
	std::vector<std::unique_ptr<IRequest>> m_requests;
};