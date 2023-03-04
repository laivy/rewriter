#include "stdafx.h"
#include "NytApp.h"
#include "NytLoader.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "NytUI.h"

NytProperty* NytLoader::Load(const std::string& filePath)
{
	// 이미 로딩된 데이터인지 확인
	if (m_data.contains(filePath))
		return m_data[filePath].get();

	std::ifstream ifstream{ StringTable::DATA_FOLDER_PATH + filePath, std::ifstream::binary };
	assert(ifstream);

	// 루트 프로퍼티 개수
	int nodeCount{};
	ifstream.read(reinterpret_cast<char*>(&nodeCount), sizeof(int));

	// 순회하며 모든 프로퍼티 로딩
	std::unique_ptr<NytProperty> root{ new NytProperty };
	for (int i = 0; i < nodeCount; ++i)
	{
		Load(ifstream, root.get());
	}

	// 저장 후 반환
	m_data[filePath] = std::move(root);
	return m_data[filePath].get();
}

void NytLoader::Unload(const std::string& filePath)
{
	if (m_data.contains(filePath))
		m_data.erase(filePath);
}

void NytLoader::CreateShaderResourceView()
{
	m_srvHeap.Reset();

	auto d3dDevice{ NytApp::GetInstance()->GetD3DDevice() };

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = m_srvData.size();
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvDescriptorHandle{ m_srvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (const auto& resource : m_srvData)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3dDevice->CreateShaderResourceView(resource, &srvDesc, srvDescriptorHandle);
		srvDescriptorHandle.Offset(g_cbvSrvUavDescriptorIncrementSize);
	}
}

void NytLoader::ReleaseUploadBuffers()
{
	m_uploadBuffers.clear();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE NytLoader::GetGPUDescriptorHandle(ID3D12Resource* resource)
{
	auto it{ m_srvData.find(resource) };
	if (it == m_srvData.end())
		assert(false);

	UINT index{ static_cast<UINT>(std::distance(m_srvData.begin(), it)) };
	CD3DX12_GPU_DESCRIPTOR_HANDLE handle{ m_srvHeap->GetGPUDescriptorHandleForHeapStart() };
	handle.Offset(index, g_cbvSrvUavDescriptorIncrementSize);
	return handle;
}

void NytLoader::Load(std::ifstream& fs, NytProperty* root)
{
	NytDataType type{ Read<BYTE>(fs) };
	std::string name{ Read<std::string>(fs) };

	std::any data{};
	switch (type)
	{
	case NytDataType::GROUP:
		break;
	case NytDataType::INT:
		data = new int{ Read<int>(fs) };
		break;
	case NytDataType::INT2:
		data = new INT2{ Read<INT2>(fs) };
		break;
	case NytDataType::FLOAT:
		data = new float{ Read<float>(fs) };
		break;
	case NytDataType::STRING:
		data = new std::string{ Read<std::string>(fs) };
		break;
	case NytDataType::UI:
		data = new NytUI{ Read<NytUI>(fs) };
		break;
	case NytDataType::IMAGE:
		data = new NytImage{ Read<NytImage>(fs) };
		break;
	default:
		assert(false);
	}

	int childNodeCount{ Read<int>(fs) };
	root->m_childNames.reserve(childNodeCount);
	root->m_childNames.push_back(name);
	root->m_childProps[name] = std::make_unique<NytProperty>(type, data);
	for (int i = 0; i < childNodeCount; ++i)
	{
		Load(fs, root->m_childProps[name].get());
	}
}

template<>
std::string NytLoader::Read(std::ifstream& fs)
{
	char length{};
	char buffer[50]{};
	fs.read(&length, sizeof(char));
	fs.read(buffer, length);
	return std::string{ buffer };
}

template<>
NytUI NytLoader::Read(std::ifstream& fs)
{
	int length{ Read<int>(fs) };
	std::unique_ptr<BYTE> buffer{ new BYTE[length] };
	fs.read(reinterpret_cast<char*>(buffer.get()), length);

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

	auto d2dContext{ NytApp::GetInstance()->GetD2DContext() };
	hr = d2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bitmap);
	assert(SUCCEEDED(hr));

	return NytUI{ bitmap };
}

template<>
NytImage NytLoader::Read(std::ifstream& fs)
{
	int length{ Read<int>(fs) };
	std::unique_ptr<BYTE> buffer{ new BYTE[length] };
	fs.read(reinterpret_cast<char*>(buffer.get()), length);

	ID3D12Resource* bitmap;
	ComPtr<ID3D12Resource> uploadBuffer;
	auto d3dDevice{ NytApp::GetInstance()->GetD3DDevice() };
	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA subresource;
	DirectX::LoadWICTextureFromMemory(
		d3dDevice.Get(),
		buffer.get(),
		length,
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

	auto commandList{ NytApp::GetInstance()->GetCommandList() };
	UpdateSubresources(commandList.Get(), bitmap, uploadBuffer.Get(), 0, 0, 1, &subresource);

	// GPU 메모리에 복사가 끝난 뒤에 해제해야함
	m_uploadBuffers.push_back(uploadBuffer);

	// 나중에 SRV 만들 때 사용됨
	m_srvData.insert(bitmap);

	return NytImage{ bitmap };
}