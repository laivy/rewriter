#include "Stdafx.h"
#include "NytImage.h"
#include "NytLoader.h"

NytImage::NytImage(const ComPtr<ID3D12Resource>& resource) : m_resource{ resource }
{
	auto desc{ resource->GetDesc() };
	m_size.x = desc.Width;
	m_size.y = desc.Height;
}

void NytImage::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, UINT rootParameterIndex)
{
	auto handle{ NytLoader::GetInstance()->GetGPUDescriptorHandle(m_resource.Get()) };
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);
}

ID3D12Resource* NytImage::GetResource() const
{
	return m_resource.Get();
}
