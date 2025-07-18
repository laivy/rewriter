module rewriter.library.resource;

import std;
import :global;
import :model;
import :sprite;

namespace Resource
{
	void Initialize(
		const std::filesystem::path& mountPath,
		const std::function<std::shared_ptr<Sprite>(std::span<std::byte>)>& loadSprite,
		const std::function<std::shared_ptr<Model>(std::span<std::byte>)>& loadModel
	)
	{
		g_mountPath = mountPath;
		g_loadSprite = loadSprite;
		g_loadModel = loadModel;
	}

	void CleanUp()
	{
		g_resources.clear();
		g_mountPath.clear();
		g_loadSprite = nullptr;
		g_loadModel = nullptr;
	}
}
