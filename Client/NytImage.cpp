#include "Stdafx.h"
#include "NytImage.h"
#include "ResourceManager.h"

NytImage::NytImage(ID3D12Resource* resource) : m_resource{ resource }
{
	auto desc{ resource->GetDesc() };

	m_cbTexture.Init();
	m_cbTexture->width = static_cast<UINT>(desc.Width);
	m_cbTexture->height = static_cast<UINT>(desc.Height);
}

void NytImage::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex)
{
	auto handle{ ResourceManager::GetInstance()->GetGPUDescriptorHandle(m_resource.Get()) };
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
	commandList->SetGraphicsRootConstantBufferView(RootParamIndex::TEXTURE, m_cbTexture.GetGPUVirtualAddress());
}