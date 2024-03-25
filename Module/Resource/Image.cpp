#include "Stdafx.h"
#include "Include/Image.h"
#include <d2d1_3.h>
#include <d3d12.h>
#include <wincodec.h>

namespace Resource
{
	Image::Image() : m_bufferSize{ 0 }
	{
	}

	Image::~Image()
	{
	}

	void Image::SetBuffer(std::byte* buffer, size_t size)
	{
		m_buffer.reset(buffer);
		m_bufferSize = size;
	}

	void Image::SetD2DBitmap(ID2D1Bitmap* bitmap)
	{
		m_d2dBitmap = bitmap;
		m_buffer.reset();
		m_bufferSize = 0;
	}

	std::span<std::byte> Image::GetBuffer() const
	{
		return std::span{ m_buffer.get(), m_bufferSize };
	}

	ID2D1Bitmap* Image::GetD2DBitmap() const
	{
		return m_d2dBitmap.Get();
	}

	INT2 Image::GetSize() const
	{
		if (m_d2dBitmap)
		{
			auto [w, h] { m_d2dBitmap->GetSize() };
			return INT2{ static_cast<int>(w), static_cast<int>(h) };
		}

		if (m_d3dResource)
		{
			auto desc{ m_d3dResource->GetDesc() };
			return INT2{ static_cast<int>(desc.Width), static_cast<int>(desc.Height) };
		}

		return INT2{};
	}

	void Image::UseAs(const ComPtr<ID2D1DeviceContext2>& ctx, Type type)
	{
		if (!ctx)
			return;

		if (m_d2dBitmap)
			return;

		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<IWICStream> stream;

		HRESULT hr{ E_FAIL };
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
		hr = factory->CreateStream(&stream);
		hr = stream->InitializeFromMemory(
			reinterpret_cast<WICInProcPointer>(m_buffer.get()),
			static_cast<DWORD>(m_bufferSize)
		);
		hr = factory->CreateDecoderFromStream(stream.Get(), NULL, WICDecodeMetadataCacheOnLoad, &decoder);
		hr = factory->CreateFormatConverter(&converter);
		hr = decoder->GetFrame(0, &frameDecode);
		hr = converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
		hr = ctx->CreateBitmapFromWicBitmap(converter.Get(), &m_d2dBitmap);
		assert(SUCCEEDED(hr));
	}
}