#include "Stdafx.h"
#include "ImageLoader.h"
#include "NytApp.h"

ImageLoader::ImageLoader()
{
	HRESULT hr{ E_FAIL };
	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_WICIFactory));
	assert(SUCCEEDED(hr));
}

std::unique_ptr<Image> ImageLoader::Load(const std::wstring& fileName)
{
	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICFormatConverter> converter;
	ComPtr<IWICBitmapFrameDecode> frameDecode;
	ComPtr<ID2D1Bitmap> bitmap;

	HRESULT hr{ E_FAIL };
	hr = m_WICIFactory->CreateDecoderFromFilename(fileName.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
	hr = m_WICIFactory->CreateFormatConverter(&converter);
	hr = decoder->GetFrame(0, &frameDecode);
	hr = converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);

	auto renderTarget{ NytApp::GetInstance()->GetRenderTarget() };
	hr = renderTarget->CreateBitmapFromWicBitmap(converter.Get(), NULL, &bitmap);
	assert(SUCCEEDED(hr));

	return std::make_unique<Image>(bitmap);
}
