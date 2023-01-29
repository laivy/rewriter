#include "stdafx.h"
#include "NytApp.h"
#include "NytLoader.h"
#include "NytProperty.h"
#include "NytImage.h"

NytLoader::NytLoader()
{
	HRESULT hr{ E_FAIL };
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_WICIFactory));
	assert(SUCCEEDED(hr));
}

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
	case NytDataType::FLOAT:
	{
		data = Read<float>(fs);
		break;
	}
	case NytDataType::STRING:
		data = Read<std::string>(fs);
		break;
	case NytDataType::IMAGE:
	{
		data = Read<NytImage>(fs);
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

template<>
NytImage NytLoader::Read(std::ifstream& fs)
{
	int length{ Read<int>(fs) };

	std::unique_ptr<BYTE> buffer{ new BYTE[length] };
	fs.read(reinterpret_cast<char*>(buffer.get()), length);

	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICFormatConverter> converter;
	ComPtr<IWICBitmapFrameDecode> frameDecode;
	ComPtr<IWICStream> stream;
	ComPtr<ID2D1Bitmap> bitmap;

	HRESULT hr{ E_FAIL };
	hr = m_WICIFactory->CreateStream(&stream);
	hr = stream->InitializeFromMemory(buffer.get(), length);
	hr = m_WICIFactory->CreateDecoderFromStream(stream.Get(), NULL, WICDecodeMetadataCacheOnLoad, &decoder);
	hr = m_WICIFactory->CreateFormatConverter(&converter);
	hr = decoder->GetFrame(0, &frameDecode);
	hr = converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);

	auto renderTarget{ NytApp::GetInstance()->GetRenderTarget() };
	hr = renderTarget->CreateBitmapFromWicBitmap(converter.Get(), NULL, &bitmap);
	assert(SUCCEEDED(hr));

	return NytImage{ bitmap };
}