#include "Stdafx.h"
#include "ServerSocket.h"

void ServerSocket::OnLoginServerPacket(Packet& packet)
{
	switch (packet.GetType())
	{
	case Protocol::Type::Register:
	{
		OnRegisterAccount(packet);
		break;
	}
	default:
		Disconnect();
		break;
	}
}

void ServerSocket::OnRegisterAccount(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Register>() };
	switch (subType)
	{
	case Protocol::Register::CheckID:
	{
		auto socketID{ packet.Decode<ID>() };

		int32_t isAvailable{};
		auto name{ packet.Decode<std::wstring>() };
		Database::StoredProcedure{ Database::Type::Game }
			.Statement(L"{ ? = CALL [dbo].[is_available_account_name] (?) }")
			.Out(1, &isAvailable)
			.In(2, name)
			.Execute();

		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::CheckID, socketID, isAvailable == 1);
		Send(outPacket);
		break;
	}
	default:
		break;
	}
}
