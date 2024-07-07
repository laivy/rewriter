#include "Stdafx.h"
#include "User.h"

User::User(const std::shared_ptr<Socket>& socket) :
	m_socket{ socket },
	m_account{}
{
}

void User::OnPacket(Packet& packet)
{
}

void User::Update(float deltaTime)
{
}

const std::shared_ptr<Socket>& User::GetSocket() const
{
	return m_socket;
}

std::uint32_t User::GetAccountID() const
{
	return m_account.id;
}
