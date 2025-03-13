#include "Stdafx.h"
#include "ServerSocket.h"

void ServerSocket::OnLoginServerPacket(Packet& packet)
{
	switch (packet.GetType())
	{
	case Protocol::Type::Login:
	{
		OnLoginRequest(packet);
		break;
	}
	case Protocol::Type::Register:
	{
		OnRegisterAccountRequest(packet);
		break;
	}
	default:
		Disconnect();
		break;
	}
}

void ServerSocket::OnLoginRequest(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Login>() };
	switch (subType)
	{
	case Protocol::Login::Request:
	{
		auto socketID{ packet.Decode<ID>() };

		int32_t ret{ -1 };
		auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };
		Database::StoredProcedure{ Database::Type::Game }
			.Statement(L"{ ? = CALL [dbo].[login] (?, ?) }")
			.Out(1, &ret)
			.In(2, id)
			.In(3, pw)
			.Execute();

		Packet outPacket{ Protocol::Type::Login };
		outPacket.Encode(Protocol::Login::Result, socketID, ret == 0);
		Send(outPacket);
		break;
	}
	default:
		break;
	}
}

void ServerSocket::OnRegisterAccountRequest(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Register>() };
	switch (subType)
	{
	case Protocol::Register::Check:
	{
		auto socketID{ packet.Decode<ID>() };

		int32_t ret{ -1 };
		auto id{ packet.Decode<std::wstring>() };
		Database::StoredProcedure{ Database::Type::Game }
			.Statement(L"{ ? = CALL [dbo].[is_available_account_name] (?) }")
			.Out(1, &ret)
			.In(2, id)
			.Execute();

		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::CheckResult, socketID, ret == 0);
		Send(outPacket);
		break;
	}
	case Protocol::Register::Request:
	{
		auto socketID{ packet.Decode<ID>() };

		int32_t ret{ -1 };
		auto [id, pw] { packet.Decode<std::wstring, std::wstring>() };
		Database::StoredProcedure{ Database::Type::Game }
			.Statement(L"{ ? = CALL [dbo].[register_account] (?, ?) }")
			.Out(1, &ret)
			.In(2, id)
			.In(3, pw)
			.Execute();

		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::Result, socketID, ret == 0);
		Send(outPacket);
		break;
	}
	default:
		break;
	}
}
