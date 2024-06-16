#pragma once

struct Context : public TSingleton<Context>
{
	int accountID{ -1 };
	int characterID{ -1 };
};