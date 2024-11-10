#include "Stdafx.h"
#include "Connection.h"
#include "Database.h"

namespace Database
{
	std::map<Database, Connection> g_connections;

	DLL_API void Initialize(const std::shared_ptr<Resource::Property>& prop)
	{
		if (!prop)
		{
			assert(false);
			return;
		}

		for (const auto& [k, v] : *prop)
		{
			auto database{ Database::None };
			if (k == L"Game")
				database = Database::Game;
			else if (k == L"World")
				database = Database::World;

			if (database == Database::None)
			{
				assert(false);
				continue;
			}

			auto server{ v->GetString(L"Server") };
			auto name{ v->GetString(L"Name") };
			auto id{ v->GetString(L"ID") };
			auto pw{ v->GetString(L"PW") };
			g_connections.emplace(database, Connection{ server, name, id, pw });
		}
	}
}
