#include "Stdafx.h"
#include "NytImage.h"
#include "ResourceManager.h"

NytImage::NytImage(ID2D1Bitmap* bitmap) : m_bitmap{ bitmap }
{
	m_size.x = bitmap->GetSize().width;
	m_size.y = bitmap->GetSize().height;
}

NytImage::NytImage(ID3D12Resource* resource) : m_resource{ resource }
{
	auto desc{ resource->GetDesc() };

	m_cbTexture.Init();
	m_cbTexture->width = static_cast<UINT>(desc.Width);
	m_cbTexture->height = static_cast<UINT>(desc.Height);
}

void NytImage::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext, FLOAT x, FLOAT y) const
{
	d2dContext->DrawBitmap(m_bitmap.Get(), RECTF{ x, y, x + m_size.x, y + m_size.y });
}

void NytImage::SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex)
{
	auto handle{ ResourceManager::GetInstance()->GetGPUDescriptorHandle(m_resource.Get()) };
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
	m_cbTexture.SetShaderVariable(commandList, RootParamIndex::TEXTURE);
}