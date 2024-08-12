#include "Stdafx.h"
#include "Global.h"

std::vector<Root> g_roots;
std::vector<std::weak_ptr<Resource::Property>> g_selectedPropertise;

void ForEachProperty(const std::shared_ptr<Resource::Property>& prop, const std::function<void(const std::shared_ptr<Resource::Property>&)>& func)
{
	for (const auto& child : prop->GetChildren())
		ForEachProperty(child, func);
	func(prop);
}

void DeleteProperty(const std::shared_ptr<Resource::Property>& prop)
{
	for (const auto& root : g_roots)
		ForEachProperty(root.prop, [&prop](const auto& p) { std::erase(p->GetChildren(), prop); });
	std::erase_if(g_roots, [&prop](const auto& root) { return root.prop == prop; });
}

Root& GetRoot(const std::shared_ptr<Resource::Property>& prop)
{
	auto it{ std::ranges::find_if(g_roots, [&prop](const auto& root) { return root.prop == prop; }) };
	return *it;
}

std::shared_ptr<Resource::Property> GetParent(const std::shared_ptr<Resource::Property>& prop)
{
	std::shared_ptr<Resource::Property> parent;
	for (const auto& root : g_roots)
		ForEachProperty(root.prop,
			[&prop, &parent](const auto& p)
			{
				const auto& children{ p->GetChildren() };
				auto it{ std::ranges::find_if(children, [&prop](const auto& child) { return child == prop; }) };
				if (it != children.end())
					parent = p;
			});
	return parent;
}

bool IsRoot(const std::shared_ptr<Resource::Property>& prop)
{
	return std::ranges::find_if(g_roots, [&prop](const auto& root) { return root.prop == prop; }) != g_roots.end();
}

bool IsSelected(const std::shared_ptr<Resource::Property>& prop)
{
	return std::ranges::find_if(g_selectedPropertise, [&prop](const auto& p) { return p.lock() == prop; }) != g_selectedPropertise.end();
}