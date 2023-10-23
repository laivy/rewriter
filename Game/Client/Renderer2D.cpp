#include "Stdafx.h"
#include "ClientApp.h"
#include "Renderer2D.h"

namespace
{
	ComPtr<ID2D1Bitmap> LoadD2DImage(const std::vector<BYTE>& binary)
	{
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<IWICStream> stream;
		ID2D1Bitmap* bitmap{};

		HRESULT hr{ E_FAIL };
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
		hr = factory->CreateStream(&stream);
		hr = stream->InitializeFromMemory(const_cast<BYTE*>(binary.data()), static_cast<DWORD>(binary.size()));
		hr = factory->CreateDecoderFromStream(stream.Get(), NULL, WICDecodeMetadataCacheOnLoad, &decoder);
		hr = factory->CreateFormatConverter(&converter);
		hr = decoder->GetFrame(0, &frameDecode);
		hr = converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
		hr = Renderer2D::g_ctx->CreateBitmapFromWicBitmap(converter.Get(), &bitmap);
		assert(SUCCEEDED(hr));

		return bitmap;
	}
}

namespace Renderer2D
{
	ComPtr<ID2D1DeviceContext2> g_ctx{};

	void DrawImage(const std::shared_ptr<Resource::Property>& prop, const INT2& position, float opacity)
	{
		if (!prop)
			return;

		auto image{ Resource::GetD2DImage(prop) };
		if (!image)
		{
			image = LoadD2DImage(Resource::GetBinary(prop));
			Resource::SetD2DImage(prop, image);
		}

		auto [w, h] { image->GetSize() };

		g_ctx->DrawBitmap(
			image.Get(),
			RECTF{ static_cast<float>(position.x), static_cast<float>(position.y), position.x + w, position.y + h },
			opacity
		);
	}
}