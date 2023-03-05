#include "Stdafx.h"
#include "NytImage.h"
#include "NytLoader.h"

NytImage::NytImage(ID3D12Resource* resource) : m_resource{ resource }
{
	auto desc{ resource->GetDesc() };

	m_cbImage.Init();
	m_cbImage->size.x = desc.Width;
	m_cbImage->size.y = desc.Height;
}

void NytImage::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex)
{
	auto handle{ NytLoader::GetInstance()->GetGPUDescriptorHandle(m_resource.Get()) };
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
	commandList->SetGraphicsRootConstantBufferView(RootParamIndex::TEXTURE, m_cbImage.GetGPUVirtualAddress());
}

ID3D12Resource* NytImage::GetResource() const
{
	return m_resource.Get();
}
