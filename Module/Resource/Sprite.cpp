#include "Stdafx.h"
#if defined _CLIENT || defined _TOOL
#include "Global.h"
#include "Resource.h"
#include "Sprite.h"

namespace Resource
{
	DLL_API Sprite::Sprite()
	{
#ifdef _TOOL
		m_binarySize = 0;
#endif
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

#ifdef _TOOL
		// 나중에 저장하기 위해 바이너리 데이터 복사
		m_binary.reset(new std::byte[binary.size()]{});
		std::memcpy(m_binary.get(), binary.data(), binary.size());
		m_binarySize = static_cast<uint32_t>(binary.size());
#endif
	}

	DLL_API ID2D1Bitmap* Sprite::Get() const
	{
		return m_bitmap.Get();
	}

	DLL_API INT2 Sprite::GetSize() const
	{
		if (!m_bitmap)
			return INT2{ 0, 0 };

		auto [w, h] { m_bitmap->GetSize() };
		return INT2{ static_cast<int32_t>(w), static_cast<int32_t>(h) };
	}

#ifdef _TOOL
	DLL_API uint32_t Sprite::GetBinarySize() const
	{
		return m_binarySize;
	}

	DLL_API std::byte* Sprite::GetBinary() const
	{
		return m_binary.get();
	}
#endif
}
#endif
