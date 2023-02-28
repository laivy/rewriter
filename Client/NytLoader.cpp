#include "stdafx.h"
#include "NytApp.h"
#include "NytLoader.h"
#include "NytProperty.h"
#include "NytImage.h"

NytProperty& NytLoader::Load(const std::string& filePath)
{
	// 이미 로딩된 데이터인지 확인
	if (m_data.contains(filePath))
		return m_data[filePath];

	std::ifstream ifstream{ StringTable::DATA_FOLDER_PATH + filePath, std::ifstream::binary };
	assert(ifstream);

	// 루트 프로퍼티 개수
	int nodeCount{};
	ifstream.read(reinterpret_cast<char*>(&nodeCount), sizeof(int));

	// 순회하며 모든 프로퍼티 로딩
	NytProperty root{};
	for (int i = 0; i < nodeCount; ++i)
	{
		Load(ifstream, root);
	}

	// 저장 후 반환
	m_data[filePath] = std::move(root);
	return m_data[filePath];
}

void NytLoader::Unload(const std::string& filePath)
{
	if (m_data.contains(filePath))
		m_data.erase(filePath);
}

void NytLoader::ClearUploadBuffers()
{
	m_uploadBuffers.clear();
}

void NytLoader::Load(std::ifstream& fs, NytProperty& root)
{
	NytDataType type{ Read<BYTE>(fs) };
	std::string name{ Read<std::string>(fs) };

	std::any data{};
	switch (type)
	{
	case NytDataType::GROUP:
		break;
	case NytDataType::INT:
	{
		data = Read<int>(fs);
		break;
	}
	case NytDataType::INT2:
	{
		data = Read<INT2>(fs);
		break;
	}
	case NytDataType::FLOAT:
	{
		data = Read<float>(fs);
		break;
	}
	case NytDataType::STRING:
		data = Read<std::string>(fs);
		break;
	case NytDataType::UI:
	case NytDataType::IMAGE:
	{
		data = Read(fs, type);
		break;
	}
	default:
		assert(false);
	}

	int childNodeCount{ Read<int>(fs) };
	root.m_childNames.reserve(childNodeCount);
	root.m_childNames.push_back(name);
	root.m_childProps[name] = NytProperty{ type, data };
	for (int i = 0; i < childNodeCount; ++i)
	{
		Load(fs, root.m_childProps[name]);
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

NytImage NytLoader::Read(std::ifstream& fs, NytDataType type)
{
	int length{ Read<int>(fs) };
	std::unique_ptr<BYTE> buffer{ new BYTE[length] };
	fs.read(reinterpret_cast<char*>(buffer.get()), length);
	
	switch (type)
	{
	case NytDataType::UI:
	{
		// Direct2D
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<IWICStream> stream;
		ComPtr<ID2D1Bitmap> bitmap;

		HRESULT hr{ E_FAIL };
		hr = factory->CreateStream(&stream);
		hr = stream->InitializeFromMemory(buffer.get(), length);
		hr = factory->CreateDecoderFromStream(stream.Get(), NULL, WICDecodeMetadataCacheOnLoad, &decoder);
		hr = factory->CreateFormatConverter(&converter);
		hr = decoder->GetFrame(0, &frameDecode);
		hr = converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);

		auto d2dContext{ NytApp::GetInstance()->GetD2DContext() };
		hr = d2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bitmap);
		assert(SUCCEEDED(hr));

		return NytImage{ bitmap };
	}
	case NytDataType::IMAGE:
	{
		ComPtr<ID3D12Resource> bitmap, uploadBuffer;
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

		UINT64 nBytes{ GetRequiredIntermediateSize(bitmap.Get(), 0, 1) };
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
		UpdateSubresources(commandList.Get(), bitmap.Get(), uploadBuffer.Get(), 0, 0, 1, &subresource);
		m_uploadBuffers.push_back(uploadBuffer);

		return NytImage{ bitmap };
	}
	default:
		assert(false);
	}
}