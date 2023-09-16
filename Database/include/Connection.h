#pragma once

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

namespace Database
{
	enum class DBRESULT
	{
		SUCCESS,
		E_NOT_INITIALIZED,
		E_UNKNOWN
	};

	class Connection : public TSingleton<Connection>
	{
	public:
		Connection();
		~Connection() = default;

		void OnCreate();

	public:
		SQLHENV m_hEnv;
		SQLHDBC m_hDbc;
	};

	DLLEXPORT DBRESULT Connect(const std::wstring& server, const std::wstring& database, const std::wstring& username, const std::wstring& password);
}