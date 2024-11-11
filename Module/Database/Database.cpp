#include "Stdafx.h"
#include "Database.h"
#include "Query.h"
#include "Session.h"

namespace Database
{
	DLL_API void Initialize(const std::shared_ptr<Resource::Property>& prop)
	{
		if (!prop)
		{
			assert(false);
			return;
		}

		for (const auto& [k, v] : *prop)
		{
			auto database{ Type::None };
			if (k == L"Game")
				database = Type::Game;
			else if (k == L"World")
				database = Type::World;

			if (database == Type::None)
			{
				assert(false);
				continue;
			}

			auto server{ v->GetString(L"Server") };
			auto name{ v->GetString(L"Name") };
			auto id{ v->GetString(L"ID") };
			auto pw{ v->GetString(L"PW") };
			Connect(database, server, name, id, pw);
		}
	}
}
