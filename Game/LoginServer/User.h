#pragma once

using AccountID = unsigned int;

struct Account
{
	AccountID id;
	std::string name;
};

class User
{
public:
	User() = default;
	~User() = default;

	AccountID GetAccountID() const;

public:
	Account m_account;
};