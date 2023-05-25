#include "Stdafx.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	m_shaderResources.reserve(SRV_HEAP_COUNT);

	CreateSRVHeap();
	CreateShaders();
}

ResourceManager::~ResourceManager()
{

}

Property* ResourceManager::Load(const std::string& filePath)
{
	// 이미 로딩된 데이터인지 확인
	if (m_properties.contains(filePath))
		return m_properties[filePath].get();

	// 파일 로드
	std::ifstream ifstream{ StringTable::DATA_FOLDER_PATH + filePath, std::ifstream::binary };
	assert(ifstream);

	// 루트 노드
	std::unique_ptr<Property> root{ new Property };

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

	auto d3dDevice{ GameApp::GetInstance()->GetD3DDevice() };
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
	auto d3dDevice{ GameApp::GetInstance()->GetD3DDevice() };

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = static_cast<UINT>(SRV_HEAP_COUNT);
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));
}

void ResourceManager::CreateShaders()
{
	m_shaders.emplace(Shader::Type::DEFAULT, new Shader);
}

void ResourceManager::Load(std::ifstream& fs, Property* root)
{
	Property::Type type{ Read<BYTE>(fs) };
	std::string name{ Read<std::string>(fs) };
	std::any data{};

	switch (type)
	{
	case Property::Type::GROUP:
		break;
	case Property::Type::INT:
		data = new int{ Read<int>(fs) };
		break;
	case Property::Type::INT2:
		data = new INT2{ Read<INT2>(fs) };
		break;
	case Property::Type::FLOAT:
		data = new float{ Read<float>(fs) };
		break;
	case Property::Type::STRING:
		data = new std::string{ Read<std::string>(fs) };
		break;
	case Property::Type::D2DImage:
	case Property::Type::D3DImage:
		data = new Image{ Read(fs, type) };
		break;
	default:
		assert(false);
	}

	root->m_childProps.emplace(name, new Property{ type, data });

	int childNodeCount{ Read<int>(fs) };
	for (int i = 0; i < childNodeCount; ++i)
		Load(fs, root->m_childProps[name].get());
}

Image ResourceManager::Read(std::ifstream& fs, Property::Type type)
{
	int length{ Read<int>(fs) };
	std::unique_ptr<BYTE> buffer{ new BYTE[length] };
	fs.read(reinterpret_cast<char*>(buffer.get()), length);

	if (type == Property::Type::D2DImage)
	{
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<IWICStream> stream;
		ID2D1Bitmap* bitmap;

		HRESULT hr{ E_FAIL };
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
		hr = factory->CreateStream(&stream);
		hr = stream->InitializeFromMemory(buffer.get(), length);
		hr = factory->CreateDecoderFromStream(stream.Get(), NULL, WICDecodeMetadataCacheOnLoad, &decoder);
		hr = factory->CreateFormatConverter(&converter);
		hr = decoder->GetFrame(0, &frameDecode);
		hr = converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);

		auto d2dContext{ GameApp::GetInstance()->GetD2DContext() };
		hr = d2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bitmap);
		assert(SUCCEEDED(hr));

		return Image{ bitmap };
	}
	else if (type == Property::Type::D3DImage)
	{
		ID3D12Resource* bitmap;
		ComPtr<ID3D12Resource> uploadBuffer;
		auto d3dDevice{ GameApp::GetInstance()->GetD3DDevice() };
		std::unique_ptr<uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA subresource;
		DirectX::LoadWICTextureFromMemoryEx(
			d3dDevice,
			buffer.get(),
			length,
			0,
			D3D12_RESOURCE_FLAG_NONE,
			DirectX::WIC_LOADER_FORCE_RGBA32,
			&bitmap,
			decodedData,
			subresource
		);

		UINT64 nBytes{ GetRequiredIntermediateSize(bitmap, 0, 1) };
		nBytes += D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(nBytes),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			NULL,
			IID_PPV_ARGS(&uploadBuffer)
		));

		auto commandList{ GameApp::GetInstance()->GetCommandList() };
		UpdateSubresources(commandList, bitmap, uploadBuffer.Get(), 0, 0, 1, &subresource);

		// GPU 메모리에 복사가 끝난 뒤에 해제해야함
		m_uploadBuffers.push_back(uploadBuffer);

		// SRV 생성
		CreateShaderResourceView(bitmap);

		return Image{ bitmap };
	}
	assert(false);
}
