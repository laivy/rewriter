#pragma once

class Camera;
class IGameObject;
class IScene;
class LocalPlayer;
class Map;
class RemotePlayer;

class ObjectManager : public TSingleton<ObjectManager>
{
public:
	ObjectManager() = default;
	~ObjectManager() = default;

	void OnCreate();

	void Update(float deltaTime);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void AddObject(const std::shared_ptr<IGameObject>& object);
	void SetCamera(const std::shared_ptr<Camera>& camera);
	void SetMap(const std::shared_ptr<Map>& map);
	void SetLocalPlayer(const std::shared_ptr<LocalPlayer>& player);
	void AddRemotePlayer(const std::shared_ptr<RemotePlayer>& player);

	std::weak_ptr<IGameObject> GetGameObject(IGameObject* object) const;
	std::weak_ptr<Camera> GetCamera() const;
	std::weak_ptr<Map> GetMap() const;
	std::weak_ptr<LocalPlayer> GetLocalPlayer() const;
	std::weak_ptr<RemotePlayer> GetRemotePlayer(CharacterID characterID) const;

private:
	// 이벤트 함수들
	bool OnSceneChange(IScene* scene);

	void RemoveInvalidObjects();

private:
	std::vector<std::shared_ptr<IGameObject>> m_gameObjects;

	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<Map> m_map;
	std::shared_ptr<LocalPlayer> m_localPlayer;
	std::map<CharacterID, std::shared_ptr<RemotePlayer>> m_remotePlayers;
};