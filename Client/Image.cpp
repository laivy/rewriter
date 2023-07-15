#include "Stdafx.h"
#include "Image.h"
#include "ResourceManager.h"

Image::Image()
{
	assert(false);
}

Image::Image(ID2D1Bitmap* bitmap)
{
	D2DImage data{};
	data.bitmap = bitmap;
	data.size.x = bitmap->GetSize().width;
	data.size.y = bitmap->GetSize().height;
	m_data = data;
}

Image::Image(ID3D12Resource* resource)
{
	D3DImage data{};
	data.resource = resource;

	auto desc{ resource->GetDesc() };
	data.cbImage.Init();
	data.cbImage->width = static_cast<UINT>(desc.Width);
	data.cbImage->height = static_cast<UINT>(desc.Height);

	m_data = data;
}

void Image::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext, FLOAT x, FLOAT y) const
{
	auto d2dImage{ std::get<D2DImage>(m_data) };
	d2dContext->DrawBitmap(d2dImage.bitmap.Get(), RECTF{ x, y, x + d2dImage.size.x, y + d2dImage.size.y });
}

void Image::SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex)
{
	auto d3dImage{ std::get<D3DImage>(m_data) };
	auto handle{ ResourceManager::GetInstance()->GetGPUDescriptorHandle(d3dImage.resource.Get()) };
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
	d3dImage.cbImage.SetShaderVariable(commandList, RootParamIndex::TEXTURE);
}