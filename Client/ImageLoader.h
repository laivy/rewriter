#pragma once
#include "Image.h"

class ImageLoader : public TSingleton<ImageLoader>
{
public:
	ImageLoader();

	std::unique_ptr<Image> Load(const std::wstring& fileName);

private:
	ComPtr<IWICImagingFactory> m_WICIFactory;
};