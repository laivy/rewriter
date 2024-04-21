#include "Stdafx.h"
#include "Include/Image.h"
#include <d2d1_3.h>
#include <d3d12.h>
#include <wincodec.h>

namespace Resource
{
	Image::Image() :
		m_type{ Type::NONE },
		m_bufferSize{ 0 }
	{
	}

	void Image::SetBuffer(std::byte* buffer, size_t size)
	{
		m_buffer.reset(buffer);
		m_bufferSize = size;
	}

	std::span<std::byte> Image::GetBuffer() const
	{
		return std::span{ m_buffer.get(), m_bufferSize };
	}

	IUnknown* Image::Get() const
	{
		return m_resource.Get();
	}

	INT2 Image::GetSize() const
	{
		switch (m_type)
		{
		case Type::D2D:
		{
			auto d2dBitmap{ static_cast<ID2D1Bitmap*>(m_resource.Get()) };
			auto [w, h] { d2dBitmap->GetSize() };
			return INT2{ static_cast<int>(w), static_cast<int>(h) };
		}
		case Type::D3D:
		{
			auto d3dResource{ static_cast<ID3D12Resource*>(m_resource.Get()) };
			auto desc{ d3dResource->GetDesc() };
			return INT2{ static_cast<int>(desc.Width), static_cast<int>(desc.Height) };
		}
		default:
			assert(false && "INVALID IMAGE TYPE");
			break;
		}

		return INT2{};
	}

	void Image::UseAs(const ComPtr<ID2D1DeviceContext2>& ctx, Type type)
	{
		if (!ctx || m_type == type)
			return;

		if (m_resource)
			return;

		ComPtr<ID2D1Bitmap> bitmap;
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<IWICStream> stream;

		HRESULT hr{ E_FAIL };
		hr = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
		hr = factory->CreateStream(&stream);
		hr = stream->InitializeFromMemory(
			reinterpret_cast<WICInProcPointer>(m_buffer.get()),
			static_cast<DWORD>(m_bufferSize)
		);
		hr = factory->CreateDecoderFromStream(stream.Get(), NULL, WICDecodeMetadataCacheOnLoad, &decoder);
		hr = factory->CreateFormatConverter(&converter);
		hr = decoder->GetFrame(0, &frameDecode);
		hr = converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
		hr = ctx->CreateBitmapFromWicBitmap(converter.Get(), &bitmap);
		assert(SUCCEEDED(hr));

		m_resource = bitmap.Detach();
		m_type = type;
		m_buffer.reset();
		m_bufferSize = 0;
	}
}