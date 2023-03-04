#pragma once

enum class Pivot
{
	LEFTTOP,	CENTERTOP,	RIGHTTOP,
	LEFTCENTER, CENTER,		RIGHTCENTER,
	LEFTBOT,	CENTERBOT,	RIGHTBOT
};

enum RootParamIndex
{
	// 상수버퍼
	GAMEOBJECT = 0,
	CAMERA = 1,
	TEXTURE = 2,

	// 서술자
	TEXTURE0 = 3,

	COUNT
};