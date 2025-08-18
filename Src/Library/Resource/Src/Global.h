#pragma once

namespace Resource
{
	struct Property;
	struct Sprite;
	struct Model;
}

extern std::unordered_map<std::filesystem::path, std::shared_ptr<Resource::Property>> g_resources;

extern std::filesystem::path g_mountPath;
extern std::function<std::shared_ptr<Resource::Sprite>(std::span<std::byte>)> g_loadSprite;
extern std::function<std::shared_ptr<Resource::Model>(std::span<std::byte>)> g_loadModel;
