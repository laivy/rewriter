#pragma once
#include "Mesh.h"
#include "Shader.h"

class NytProperty;

class ResourceManager : public TSingleton<ResourceManager>
{
public:
	ResourceManager() = default;
	~ResourceManager() = default;

	void AddMesh(Mesh::Type key, Mesh* value);
	void AddShader(Shader::Type key, Shader* value);

	Mesh* GetMesh(Mesh::Type key) const;
	Shader* GetShader(Shader::Type key) const;

private:
	std::unordered_map<Mesh::Type, std::unique_ptr<Mesh>> m_meshes;
	std::unordered_map<Shader::Type, std::unique_ptr<Shader>> m_shaders;
};