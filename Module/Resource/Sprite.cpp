#include "Stdafx.h"
#if defined _CLIENT || defined _TOOL
#include "Global.h"
#include "Resource.h"
#include "Sprite.h"

namespace Resource
{
	DLL_API Sprite::Sprite() :
		m_size{}
	{
	}

	DLL_API Sprite::Sprite(std::span<std::byte> binary)
	{
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICStream> stream;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;

		if (FAILED(::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))))
			return;
		if (FAILED(factory->CreateStream(&stream)))
			return;
		if (FAILED(stream->InitializeFromMemory(reinterpret_cast<WICInProcPointer>(binary.data()), static_cast<DWORD>(binary.size()))))
			return;
		if (FAILED(factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder)))
			return;
		if (FAILED(factory->CreateFormatConverter(&converter)))
			return;
		if (FAILED(decoder->GetFrame(0, &frameDecode)))
			return;
		if (FAILED(converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut)))
			return;

		g_d2dContext->CreateBitmapFromWicBitmap(converter.Get(), m_bitmap.GetAddressOf());

		auto size{ m_bitmap->GetSize() };
		m_size = FLOAT2{ size.width, size.height };

#ifdef _TOOL
		// 나중에 저장하기 위해 바이너리 데이터 복사
		m_binary.reserve(binary.size());
		std::ranges::copy(binary, std::back_inserter(m_binary));
#endif
	}

	DLL_API ID2D1Bitmap* Sprite::Get() const
	{
		return m_bitmap.Get();
	}

	DLL_API FLOAT2 Sprite::GetSize() const
	{
		return m_size;
	}

#ifdef _TOOL
	DLL_API std::span<const std::byte> Sprite::GetBinary() const
	{
		return std::span{ m_binary.data(), m_binary.size() };
	}
#endif
}
#endif
