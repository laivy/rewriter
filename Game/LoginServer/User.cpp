#include "Stdafx.h"
#include "ClientSocket.h"
#include "User.h"

User::User(ClientSocket* socket) :
	m_socket{ socket }
{
}

void User::OnPacket(Packet& packet)
{
}

void User::Update(float deltaTime)
{
}

int User::GetAccountID() const
{
	return 0;
}
