#pragma once
#include "Image.h"
#include "Mesh.h"
#include "Property.h"
#include "Shader.h"

class ResourceManager : public TSingleton<ResourceManager>
{
public:
	ResourceManager();
	~ResourceManager();

	Property* Load(const std::string& filePath);
	void Unload(const std::string& filePath);

	ID3D12DescriptorHeap* const* GetSrvDescriptorHeap() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12Resource* const resource);
	void CreateShaderResourceView(ID3D12Resource* const resource);
	void ReleaseUploadBuffers();

	void AddMesh(Mesh::Type key, Mesh* value);
	Mesh* GetMesh(Mesh::Type key) const;

	Shader* GetShader(Shader::Type key) const;

private:
	void CreateSRVHeap();
	void CreateShaders();

	void Load(std::ifstream& fs, Property* root);

	template<class T>
	T Read(std::ifstream& fs)
	{
		T value{};
		fs.read(reinterpret_cast<char*>(&value), sizeof(T));
		return value;
	}

	template<>
	std::string Read(std::ifstream& fs)
	{
		char length{};
		fs.read(&length, sizeof(char));

		std::unique_ptr<char> buffer{ new char[length] };
		fs.read(buffer.get(), length);

		return std::string{ buffer.get(), static_cast<size_t>(length) };
	}

	Image Read(std::ifstream& fs, Property::Type type);

private:
	std::unordered_map<Mesh::Type, std::unique_ptr<Mesh>> m_meshes;
	std::unordered_map<Shader::Type, std::unique_ptr<Shader>> m_shaders;
	std::unordered_map<std::string, Property> m_properties;
	std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;

	// SRV
	enum { SRV_HEAP_COUNT = 100 };
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	std::unordered_map<ID3D12Resource*, int> m_shaderResources;
};