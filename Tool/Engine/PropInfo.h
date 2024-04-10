#pragma once

// 프로퍼티 포인터와 1:1 매칭될 정보들
// 이 프로젝트나 ImGui에서 사용하기 위한 멤버들
struct PropInfo
{
	std::weak_ptr<Resource::Property> parent;
	std::filesystem::path path;
	bool isValid{ true };
	bool isRoot{ false };
	bool isSelected{ false };
};