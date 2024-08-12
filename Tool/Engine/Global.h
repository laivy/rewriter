#pragma once

struct Root
{
	std::shared_ptr<Resource::Property> prop;
	std::filesystem::path path;
};

extern std::vector<Root> g_roots; // 모든 루트 프로퍼티들
extern std::vector<std::weak_ptr<Resource::Property>> g_selectedPropertise; // 선택된 프로퍼티들

void ForEachProperty(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func);
void DeleteProperty(const std::shared_ptr<Resource::Property>& prop);
Root& GetRoot(const std::shared_ptr<Resource::Property>& prop);
std::shared_ptr<Resource::Property> GetParent(const std::shared_ptr<Resource::Property>& prop);
bool IsRoot(const std::shared_ptr<Resource::Property>& prop);
bool IsSelected(const std::shared_ptr<Resource::Property>& prop);