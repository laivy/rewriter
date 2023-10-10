#pragma once

class IServer abstract
{
public:
	virtual bool Connect() = 0;
	virtual void Send(Packet& packet) = 0;

	virtual bool IsConnected() const = 0;
};