#include "Stdafx.h"
#include "CenterServer.h"
#include "SocketManager.h"

CenterServer::CenterServer()
{
	SetType(Type::Center);
	SocketManager::GetInstance()->Register(this);
	if (!Connect())
	{
		assert(false && "CAN NOT FIND SERVER CONFIG");
		::PostQuitMessage(0);
	}
}

void CenterServer::OnConnect(bool success)
{
	ISocket::OnConnect(success);
	if (success)
	{
		Packet packet{ Protocol::Type::Initialize };
		packet.Encode(ISocket::Type::Login);
		Send(packet);
		return;
	}

	// 재연결
	Connect();
}

void CenterServer::OnDisconnect()
{
	ISocket::OnDisconnect();

	auto sm{ SocketManager::GetInstance() };
	if (!sm)
		return;

	// 소켓 재생성
	if (Socket())
		sm->Register(this);

	// 재연결
	Connect();
}

void CenterServer::OnComplete(Packet& packet)
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
		break;
	}
}

bool CenterServer::Connect()
{
	auto prop{ Resource::Get(L"Server.dat/Login/Center") };
	if (!prop)
		return false;

	auto ip{ prop->GetString(L"IP") };
	auto port{ prop->GetInt(L"Port") };
	return ISocket::Connect(ip, port);
}

void CenterServer::OnLoginRequest(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Login>() };
	switch (subType)
	{
	case Protocol::Login::Result:
	{
		auto socketID{ packet.Decode<ID>() };
		auto sm{ SocketManager::GetInstance() };
		if (!sm)
			break;

		auto clientSocket{ sm->GetSocket(socketID) };
		if (!clientSocket)
			break;

		auto success{ packet.Decode<bool>() };
		Packet outPacket{ Protocol::Type::Login };
		outPacket.Encode(Protocol::Login::Result, success);
		clientSocket->Send(outPacket);
		break;
	}
	default:
		break;
	}
}

void CenterServer::OnRegisterAccountRequest(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Register>() };
	switch (subType)
	{
	case Protocol::Register::CheckResult:
	{
		auto socketID{ packet.Decode<ID>() };
		auto sm{ SocketManager::GetInstance() };
		if (!sm)
			break;

		auto clientSocket{ sm->GetSocket(socketID) };
		if (!clientSocket)
			break;

		auto isAvailable{ packet.Decode<bool>() };

		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::CheckResult, isAvailable);
		clientSocket->Send(outPacket);
		break;
	}
	case Protocol::Register::Result:
	{
		auto socketID{ packet.Decode<ID>() };
		auto sm{ SocketManager::GetInstance() };
		if (!sm)
			break;

		auto clientSocket{ sm->GetSocket(socketID) };
		if (!clientSocket)
			break;

		auto success{ packet.Decode<bool>() };
		Packet outPacket{ Protocol::Type::Register };
		outPacket.Encode(Protocol::Register::Result, success);
		clientSocket->Send(outPacket);
		break;
	}
	default:
		break;
	}
}
