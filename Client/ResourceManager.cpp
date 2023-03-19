#include "Stdafx.h"
#include "ResourceManager.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "NytUI.h"

ResourceManager::ResourceManager()
{
	m_shaderResources.reserve(SRV_HEAP_COUNT);

	CreateSRVHeap();
	CreateShaders();
}

ResourceManager::~ResourceManager()
{

}

NytProperty* ResourceManager::Load(const std::string& filePath)
{
	// 이미 로딩된 데이터인지 확인
	if (m_properties.contains(filePath))
		return m_properties[filePath].get();

	// 파일 로드
	std::ifstream ifstream{ StringTable::DATA_FOLDER_PATH + filePath, std::ifstream::binary };
	assert(ifstream);

	// 루트 노드
	std::unique_ptr<NytProperty> root{ new NytProperty };

	// 하위 노드 로드
	int nodeCount{ Read<int>(ifstream) };
	for (int i = 0; i < nodeCount; ++i)
		Load(ifstream, root.get());

	// 저장 후 반환
	m_properties[filePath] = std::move(root);
	return m_properties[filePath].get();
}

void ResourceManager::Unload(const std::string& filePath)
{

}

ID3D12DescriptorHeap* const* ResourceManager::GetSrvDescriptorHeap() const
{
	return m_srvHeap.GetAddressOf();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ResourceManager::GetGPUDescriptorHandle(ID3D12Resource* const resource)
{
	if (!m_shaderResources.contains(resource))
		assert(false);

	CD3DX12_GPU_DESCRIPTOR_HANDLE handle{ m_srvHeap->GetGPUDescriptorHandleForHeapStart() };
	return handle.Offset(m_shaderResources[resource], g_cbvSrvUavDescriptorIncrementSize);
}

void ResourceManager::CreateShaderResourceView(ID3D12Resource* const resource)
{
	if (m_shaderResources.contains(resource))
		return;

	auto d3dDevice{ NytApp::GetInstance()->GetD3DDevice() };
	auto index{ static_cast<int>(m_shaderResources.size()) };

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvDescriptorHandle{ m_srvHeap->GetCPUDescriptorHandleForHeapStart() };
	srvDescriptorHandle.Offset(index, g_cbvSrvUavDescriptorIncrementSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3dDevice->CreateShaderResourceView(resource, &srvDesc, srvDescriptorHandle);

	m_shaderResources[resource] = index;
}

void ResourceManager::ReleaseUploadBuffers()
{
	m_uploadBuffers.clear();
}

void ResourceManager::AddMesh(Mesh::Type key, Mesh* value)
{
	m_meshes.emplace(key, value);
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

void ResourceManager::CreateSRVHeap()
{
	auto d3dDevice{ NytApp::GetInstance()->GetD3DDevice() };

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = static_cast<UINT>(SRV_HEAP_COUNT);
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));
}

void ResourceManager::CreateShaders()
{
	m_shaders.emplace(Shader::DEFAULT, new Shader);
}

void ResourceManager::Load(std::ifstream& fs, NytProperty* root)
{
	NytType type{ Read<BYTE>(fs) };
	std::string name{ Read<std::string>(fs) };
	std::any data{};

	switch (type)
	{
	case NytType::GROUP:
		break;
	case NytType::INT:
		data = new int{ Read<int>(fs) };
		break;
	case NytType::INT2:
		data = new INT2{ Read<INT2>(fs) };
		break;
	case NytType::FLOAT:
		data = new float{ Read<float>(fs) };
		break;
	case NytType::STRING:
		data = new std::string{ Read<std::string>(fs) };
		break;
	case NytType::UI:
		data = new NytUI{ Read<NytUI>(fs) };
		break;
	case NytType::IMAGE:
		data = new NytImage{ Read<NytImage>(fs) };
		break;
	default:
		assert(false);
	}

	root->m_childNames.push_back(name);
	root->m_childProps.emplace(name, new NytProperty{ type, data });

	int childNodeCount{ Read<int>(fs) };
	for (int i = 0; i < childNodeCount; ++i)
		Load(fs, root->m_childProps[name].get());
}