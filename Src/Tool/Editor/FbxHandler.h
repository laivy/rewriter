#pragma once

namespace fbxsdk
{
	class FbxManager;
	class FbxScene;
}

class FbxHandler : public Singleton<FbxHandler>
{
public:
	FbxHandler();
	~FbxHandler();

	std::shared_ptr<Resource::Model> Load(std::filesystem::path path);

private:
	fbxsdk::FbxManager* m_manager;
	fbxsdk::FbxScene* m_scene;
};
