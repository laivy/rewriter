#include "Stdafx.h"
#include "DBManager.h"

DBManager::DBManager() :
	m_semaphores{ 0 },
	m_isActive{ false },
	m_ipport{ TEXT("localhost,1433") },
	m_dbname{ TEXT("rewriter_global") },
	m_username{ TEXT("game") },
	m_password{ TEXT("0000") }
{
	Init();
}

DBManager::~DBManager()
{
	m_isActive = false;
}

void DBManager::Render()
{
	ImGui::Begin("DBManager");
	if (ImGui::BeginTabBar("Tab"))
	{
		if (ImGui::BeginTabItem("Info"))
		{
			ImGui::TextUnformatted("Connection");
			ImGui::SameLine(100);
			ImGui::TextUnformatted(":");
			ImGui::SameLine();

			std::string str;
			ImVec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
			if (m_isActive)
			{
				str = "O";
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				str = "X";
				color = { 1.0f, 0.0f, 0.0f, 1.0f };
			}
			ImGui::TextColored(color, str.c_str());

			ImGui::TextUnformatted("IPPORT");
			ImGui::SameLine(100);
			ImGui::Text(":");
			ImGui::SameLine();
			ImGui::Text(CW2A{ m_ipport.c_str(), CP_UTF8 });

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Log"))
		{
			if (ImGui::BeginChild("LogView"))
			{
				std::unique_lock lock{ m_logMutex };
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

void DBManager::PostRequest(std::unique_ptr<IRequest> request)
{
	std::lock_guard lock{ m_requestMutex };
	m_requests.push(std::move(request));
	m_semaphores.release();
}

void DBManager::Init()
{
	// DB 연결
	auto rc{ Database::Connect(m_ipport, m_dbname, m_username, m_password) };
	if (rc == Database::DBRESULT::SUCCESS)
	{
		m_isActive = true;
	}
	else
	{
		assert(false && "FAIL TO CONNECT DATABASE");
		m_isActive = false;
	}

	// 쓰레드 생성
	for (auto& thread : m_threads)
		thread = std::jthread{ &DBManager::Run, this };
}

void DBManager::Run()
{
	while (m_isActive)
	{
		if (!m_semaphores.try_acquire_for(1s))
			continue;

		std::unique_lock lock{ m_requestMutex };
		if (m_requests.empty())
			continue;

		auto request{ std::move(m_requests.front()) };
		m_requests.pop();
		lock.unlock();

		request->Process();
	}
}