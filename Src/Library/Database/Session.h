#pragma once

namespace Database
{
	class Session
	{
	public:
		Session(std::wstring_view server, std::wstring_view database, std::wstring_view username, std::wstring_view password);
		~Session();

	public:
		SQLHENV m_env;
		SQLHDBC m_dbc;
	};

	Session* Connect(Type type, std::wstring_view server, std::wstring_view database, std::wstring_view username, std::wstring_view password);
	Session* GetSession(Type type);
}
