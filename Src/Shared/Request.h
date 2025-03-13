#pragma once

class IRequest abstract
{
public:
	virtual void Process() = 0;
};