#pragma once

class IRequest;

class DBManager : public TSingleton<DBManager>
{
public:
	DBManager();
	~DBManager();

	void Render();

	void PostRequest(std::unique_ptr<IRequest> request);

private:
	void Init();
	void Run();

private:
	static constexpr auto DB_THREAD_COUNT{ 2 };
	std::array<std::jthread, DB_THREAD_COUNT> m_threads;
	std::counting_semaphore<DB_THREAD_COUNT> m_semaphores;

	bool m_isActive;

	// DB 연결 정보
	std::wstring m_ipport;
	std::wstring m_dbname;
	std::wstring m_username;
	std::wstring m_password;

	// 로그
	std::mutex m_logMutex;
	std::vector<std::wstring> m_logs;

	// 요청
	std::mutex m_requestMutex;
	std::queue<std::unique_ptr<IRequest>> m_requests;
};