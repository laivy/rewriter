#include "Stdafx.h"
#include "CenterServer.h"
#include "SocketManager.h"

CenterServer::CenterServer()
{
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

	// 다시 연결
	Connect();
}

void CenterServer::OnDisconnect()
{
	ISocket::OnDisconnect();

	// 다시 연결
	if (!Connect())
	{
		assert(false && "CENTER SERVER DISCONNECTED");
		::PostQuitMessage(0);
	}
}

void CenterServer::OnComplete(Packet& packet)
{
	switch (packet.GetType())
	{
	case Protocol::Type::Register:
	{
		OnRegisterAccount(packet);
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

void CenterServer::OnRegisterAccount(Packet& packet)
{
	auto subType{ packet.Decode<Protocol::Register>() };
	switch (subType)
	{
	case Protocol::Register::CheckID:
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
		outPacket.Encode(Protocol::Register::CheckID, isAvailable);
		clientSocket->Send(outPacket);
		break;
	}
	default:
		break;
	}
}
