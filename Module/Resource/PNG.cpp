#include "Stdafx.h"
#if defined _CLIENT || defined _TOOL
#include <d2d1_3.h>
#include <d3d12.h>
#include <wincodec.h>
#include "PNG.h"
#include "Resource.h"

namespace Resource
{
	DLL_API PNG::PNG(std::byte* binary, size_t size)
	{
		ComPtr<ID2D1Bitmap> bitmap;
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<IWICStream> stream;

		if (FAILED(::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))))
			return;
		if (FAILED(factory->CreateStream(&stream)))
			return;
		if (FAILED(stream->InitializeFromMemory(reinterpret_cast<WICInProcPointer>(binary), static_cast<DWORD>(size))))
			return;
		if (FAILED(factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder)))
			return;
		if (FAILED(factory->CreateFormatConverter(&converter)))
			return;
		if (FAILED(decoder->GetFrame(0, &frameDecode)))
			return;
		if (FAILED(converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut)))
			return;
		g_ctx->CreateBitmapFromWicBitmap(converter.Get(), &bitmap);

#ifdef _TOOL
		// 나중에 저장하기 위해 바이너리 데이터 복사
		m_binary.reset(new std::byte[size]{});
		std::memcpy(m_binary.get(), binary, size);
		m_binarySize = size;
#endif // _TOOL
	}

	DLL_API ID2D1Bitmap* PNG::Get() const
	{
		return m_bitmap.Get();
	}

	DLL_API INT2 PNG::GetSize() const
	{
		if (!m_bitmap)
			return INT2{ 0, 0 };

		auto [w, h] { m_bitmap->GetSize() };
		return INT2{ static_cast<int32_t>(w), static_cast<int32_t>(h) };
	}

#ifdef _TOOL
	DLL_API std::byte* PNG::GetBinary() const
	{
		return m_binary.get();
	}

	DLL_API size_t PNG::GetBinarySize() const
	{
		return m_binarySize;
	}
#endif // _TOOL
}
#endif // defined _CLIENT || defined _TOOL
