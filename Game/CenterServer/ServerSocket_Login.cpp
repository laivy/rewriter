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

		int32_t ret{};
		auto id{ packet.Decode<std::wstring>() };
		Database::StoredProcedure{ Database::Type::Game }
			.Statement(L"{ ? = CALL [dbo].[is_available_account_name] (?) }")
			.Out(1, &ret)
			.In(2, id)
			.Execute();

		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::CheckID, socketID, ret == 1);
		Send(outPacket);
		break;
	}
	case Protocol::Register::Request:
	{
		auto socketID{ packet.Decode<ID>() };

		int32_t ret{};
		auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };
		Database::StoredProcedure{ Database::Type::Game }
			.Statement(L"{ ? = CALL [dbo].[register_account] (?, ?) }")
			.Out(1, &ret)
			.In(2, id)
			.In(3, pw)
			.Execute();

		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::Request, socketID, ret == 1);
		Send(outPacket);
		break;
	}
	default:
		break;
	}
}
