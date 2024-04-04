#pragma once

// 프로퍼티 포인터와 1:1 매칭될 정보들
// 이 프로젝트나 ImGui에서 사용하기 위한 멤버들
struct PropInfo
{
	std::filesystem::path path;
	bool isSelected{ false };
};