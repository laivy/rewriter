#pragma once
#include "Common/Singleton.h"

namespace Database
{
	class Connection : public TSingleton<Connection>
	{
	public:
		Connection();
		~Connection() = default;

		void OnCreate();
		void OnDestroy();

	public:
		SQLHENV m_hEnv;
		SQLHDBC m_hDbc;
	};

	__declspec(dllexport) DBRESULT Connect(const std::wstring& server, const std::wstring& database, const std::wstring& username, const std::wstring& password);
}