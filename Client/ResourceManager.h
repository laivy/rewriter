#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "NytImage.h"
#include "NytUI.h"

class NytProperty;

class ResourceManager : public TSingleton<ResourceManager>
{
public:
	ResourceManager();
	~ResourceManager() = default;

	NytProperty* Load(const std::string& filePath);
	void Unload(const std::string& filePath);

	ID3D12DescriptorHeap* const* GetSrvDescriptorHeap() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12Resource* const resource);
	void CreateShaderResourceView();
	void ReleaseUploadBuffers();

	void AddMesh(Mesh::Type key, Mesh* value);
	Mesh* GetMesh(Mesh::Type key) const;

	Shader* GetShader(Shader::Type key) const;

private:
	void CreateShaders();

	void Load(std::ifstream& fs, NytProperty* root);

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

	template<>
	NytUI Read(std::ifstream& fs)
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
	NytImage Read(std::ifstream& fs)
	{
		int length{ Read<int>(fs) };
		std::unique_ptr<BYTE> buffer{ new BYTE[length] };
		fs.read(reinterpret_cast<char*>(buffer.get()), length);

		ID3D12Resource* bitmap;
		ComPtr<ID3D12Resource> uploadBuffer;
		auto d3dDevice{ NytApp::GetInstance()->GetD3DDevice() };
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

		auto commandList{ NytApp::GetInstance()->GetCommandList() };
		UpdateSubresources(commandList, bitmap, uploadBuffer.Get(), 0, 0, 1, &subresource);

		// GPU 메모리에 복사가 끝난 뒤에 해제해야함
		m_uploadBuffers.push_back(uploadBuffer);

		// 나중에 SRV 만들 때 사용됨
		m_shaderResources.insert(bitmap);

		return NytImage{ bitmap };
	}

private:
	std::unordered_map<Mesh::Type, std::unique_ptr<Mesh>> m_meshes;
	std::unordered_map<Shader::Type, std::unique_ptr<Shader>> m_shaders;
	std::unordered_map<std::string, std::unique_ptr<NytProperty>> m_properties;

	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	std::set<ID3D12Resource*> m_shaderResources;
	std::vector<ComPtr<ID3D12Resource>> m_uploadBuffers;
};