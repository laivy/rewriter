module rewriter.library.resource:global;

import std;
import :model;
import :property;
import :sprite;

std::unordered_map<std::filesystem::path, std::shared_ptr<Resource::Property>> g_resources;

std::filesystem::path g_mountPath;
std::function<std::shared_ptr<Resource::Sprite>(std::span<std::byte>)> g_loadSprite;
std::function<std::shared_ptr<Resource::Model>(std::span<std::byte>)> g_loadModel;
