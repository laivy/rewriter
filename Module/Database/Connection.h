#pragma once

namespace Database
{
	class Connection
	{
	public:
		Connection(std::wstring_view server, std::wstring_view database, std::wstring_view username, std::wstring_view password);
		~Connection();

	public:
		SQLHENV m_hEnv;
		SQLHDBC m_hDbc;
	};
}
