#pragma once

enum class Protocol : uint16_t
{
	None,
	ServerBasicInfo,
	AccountRegisterRequest,
};

enum class AccountRegisterRequest
{
	CheckID,
	Request,
};
