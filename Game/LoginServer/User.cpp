#include "Stdafx.h"
#include "User.h"

User::User(const std::shared_ptr<Socket>& socket) :
	m_socket{ socket },
	m_account{}
{
}

void User::OnPacket(Packet& packet)
{
	switch (packet.GetType())
	{
	case Packet::Type::CLIENT_TryLogin:
	{
		std::wstring wstr;
		for (size_t i = 0; i < 1000; ++i)
			wstr += std::format(L"{} ", packet.Decode<int>());
		wstr += L"\n";
		::OutputDebugString(wstr.c_str());
		break;
	}
	default:
		break;
	}
}

const std::shared_ptr<Socket>& User::GetSocket() const
{
	return m_socket;
}

std::uint32_t User::GetAccountID() const
{
	return m_account.id;
}
