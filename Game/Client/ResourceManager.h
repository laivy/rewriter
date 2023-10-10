#pragma once
#include "Font.h"
#include "Image.h"
#include "Mesh.h"
#include "Property.h"
#include "Shader.h"

class ResourceManager : public TSingleton<ResourceManager>
{
public:
	ResourceManager();
	~ResourceManager() = default;

	Property* Load(const std::string& filePath);
	void Unload(const std::string& filePath);

	std::shared_ptr<Font> GetFont(Font::Type type) const;
	std::shared_ptr<Mesh> GetMesh(Mesh::Type type) const;
	std::shared_ptr<Shader> GetShader(Shader::Type type) const;

	ID3D12DescriptorHeap* const* GetSrvDescriptorHeap() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12Resource* const resource);

	void ReleaseUploadBuffers();

private:
	void CreateFonts();
	void CreateMeshes();
	void CreateShaders();
	void CreateSRVHeap();
	void CreateShaderResourceView(ID3D12Resource* const resource);

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

	ID2D1Bitmap* ReadD2DImage(std::ifstream& fs);
	ID3D12Resource* ReadD3DImage(std::ifstream& fs);

private:
	std::unordered_map<Font::Type, std::shared_ptr<Font>> m_fonts;
	std::unordered_map<Mesh::Type, std::shared_ptr<Mesh>> m_meshes;
	std::unordered_map<Shader::Type, std::shared_ptr<Shader>> m_shaders;
	std::unordered_map<std::string, std::unique_ptr<Property>> m_properties;

	// D3DImage
	static constexpr size_t SRV_HEAP_COUNT = 100;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;
	std::unordered_map<ID3D12Resource*, int> m_shaderResources;
};