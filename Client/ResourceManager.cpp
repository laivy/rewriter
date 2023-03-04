#include "Stdafx.h"
#include "ResourceManager.h"

void ResourceManager::AddMesh(Mesh::Type key, Mesh* value)
{
	m_meshes.emplace(key, value);
}

void ResourceManager::AddShader(Shader::Type key, Shader* value)
{
	m_shaders.emplace(key, value);
}

Mesh* ResourceManager::GetMesh(Mesh::Type key) const
{
	if (m_meshes.contains(key))
		return m_meshes.at(key).get();
	return nullptr;
}

Shader* ResourceManager::GetShader(Shader::Type key) const
{
	if (m_shaders.contains(key))
		return m_shaders.at(key).get();
	return nullptr;
}
