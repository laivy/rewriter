module Library.Resource:Global;

import std;
import :Model;
import :Property;
import :Sprite;

std::unordered_map<std::filesystem::path, std::shared_ptr<Resource::Property>> g_resources;

std::filesystem::path g_mountPath;
std::function<std::shared_ptr<Resource::Sprite>(std::span<std::byte>)> g_loadSprite;
std::function<std::shared_ptr<Resource::Model>(std::span<std::byte>)> g_loadModel;
