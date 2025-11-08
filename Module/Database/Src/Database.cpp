#include "Pch.h"
#include "Database.h"
#include "Query.h"
#include "Session.h"

namespace Database
{
	DATABASE_API void Initialize(const Resource::ID id)
	{
		for (const auto& [name, childID] : Resource::Iterator{ id })
		{
			auto type{ Type::None };
			if (name == L"Game")
				type = Type::Game;
			else if (name == L"World")
				type = Type::World;

			if (type == Type::None)
			{
				assert(false);
				continue;
			}

			const std::wstring server{ Resource::GetString(childID, L"Server").value_or(L"") };
			const std::wstring database{ Resource::GetString(childID, L"Name").value_or(L"") };
			const std::wstring username{ Resource::GetString(childID, L"ID").value_or(L"") };
			const std::wstring password{ Resource::GetString(childID, L"PW").value_or(L"") };
			if (server.empty() || database.empty() || username.empty() || password.empty())
			{
				assert(false);
				continue;
			}
			Connect(type, server, database, username, password);
		}
	}
}
