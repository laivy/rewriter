#include "Stdafx.h"
#include "Global.h"
#include "Graphics3DUtil.h"
#include "RenderTarget.h"

namespace Graphics::D3D
{
    RenderTarget::RenderTarget()
    {
		D3D12_RESOURCE_DESC textureDesc{};
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = 800;
		textureDesc.Height = 600;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		constexpr float color[]{ 0.0f, 0.0f, 0.0f, 1.0f };
		CD3DX12_CLEAR_VALUE clearValue{ DXGI_FORMAT_R8G8B8A8_UNORM, color };

		ComPtr<ID3D12Resource> textureA;
		g_d3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT },
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&clearValue,
			IID_PPV_ARGS(&textureA));
    }
}
