#pragma once

class IServer abstract
{
public:
	virtual bool Connect() = 0;
	virtual void Send(const Packet& packet) = 0;

	virtual bool IsConnected() const = 0;
};