#pragma once

enum class Protocol : unsigned short
{
	RequestRegister,
	RegisterResult,

	RequestLoginIn,
	LoginResult,
};