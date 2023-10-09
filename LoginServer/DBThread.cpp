#include "Stdafx.h"
#include "DBThread.h"

DBThread::DBThread() :
	std::thread{ &DBThread::Run, this },
	m_isActive{ true },
	m_isConnected{ false },
	m_ipport{ TEXT("localhost,1433") },
	m_dbname{ TEXT("AccountDB") },
	m_username{ TEXT("sa") },
	m_password{ TEXT("1234") }
{
}

DBThread::~DBThread()
{
	m_isActive = false;
	if (std::thread::joinable())
		std::thread::join();
}

void DBThread::Render()
{
	ImGui::Begin(CW2A{ TEXT("데이터베이스"), CP_UTF8 });
	if (ImGui::BeginTabBar("Tab"))
	{
		if (ImGui::BeginTabItem(CW2A{ TEXT("정보"), CP_UTF8 }))
		{
			ImGui::Text(CW2A{ TEXT("연결 상태"), CP_UTF8 });
			ImGui::SameLine(100);
			ImGui::Text(":");
			ImGui::SameLine();

			std::wstring str{};
			ImVec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
			if (m_isConnected)
			{
				str = TEXT("연결됨");
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				str = TEXT("연결 끊김");
				color = { 1.0f, 0.0f, 0.0f, 1.0f };
			}
			ImGui::TextColored(color, CW2A{ str.c_str(), CP_UTF8 });

			ImGui::Text(CW2A{ TEXT("아이피, 포트"), CP_UTF8 });
			ImGui::SameLine(100);
			ImGui::Text(":");
			ImGui::SameLine();
			ImGui::Text(CW2A{ m_ipport.c_str(), CP_UTF8 });

			ImGui::Text(CW2A{ TEXT("계정 이름"), CP_UTF8 });
			ImGui::SameLine(100);
			ImGui::Text(":");
			ImGui::SameLine();
			ImGui::Text(CW2A{ m_username.c_str(), CP_UTF8 });

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(CW2A{ TEXT("로그"), CP_UTF8 }))
		{
			if (ImGui::BeginChild("LogView"))
			{
				std::unique_lock lock{ m_logsMutex };
				for (const auto& log : m_logs)
				{
					ImGui::Text(CW2A{ log.c_str(), CP_UTF8 });
				}
				lock.unlock();
				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
					ImGui::SetScrollHereY(1.0f);
				ImGui::EndChild();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();
}

void DBThread::Init()
{
	auto rc{ Database::Connect(m_ipport, m_dbname, m_username, m_password) };
	if (rc != DBRESULT::SUCCESS)
	{
		assert(false && "FAIL TO CONNECT DATABASE");
		return;
	}
	m_isConnected = true;
}

void DBThread::Run()
{
	// 데이터베이스에 연결
	Init();

	HANDLE handle{ CreateEvent(NULL, TRUE, FALSE, TEXT("DBThread"))};
	while (m_isActive)
	{
		WaitForSingleObject(handle, 500);

		static int i{ 1 };
		std::wstring log{};
		std::vformat_to(
			std::back_inserter(log),
			TEXT("{}번째 로그"),
			std::make_wformat_args(i++)
		);

		if (std::unique_lock lock{ m_logsMutex })
			m_logs.push_back(log);
	}
}