#pragma once

// NytTreeNodeInfo.cs에 정의되어 있는 NytDataType와 동일해야함
enum class NytType : BYTE
{
	GROUP, INT, INT2, FLOAT, STRING, D2DImage, D3DImage
};