#include "Stdafx.h"
#include "Shader.h"

Shader::Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature)
{
	// 레이아웃 설정
	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// 셰이더 컴파일
	ComPtr<ID3DBlob> vertexShader, pixelShader, error;
#ifdef _DEBUG
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	HRESULT hr{ E_FAIL };
	hr = D3DCompileFromFile(L"default.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_1", compileFlags, 0, &vertexShader, &error);
	hr = D3DCompileFromFile(L"default.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_1", compileFlags, 0, &pixelShader, &error);
	if (FAILED(hr))
	{
		assert(false && reinterpret_cast<char*>(error->GetBufferPointer()));
	}

	// PSO 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { m_inputLayout.data(), static_cast<UINT>(m_inputLayout.size()) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE{ vertexShader.Get() };
	psoDesc.PS = CD3DX12_SHADER_BYTECODE{ pixelShader.Get() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
	psoDesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

ComPtr<ID3D12PipelineState> Shader::GetPipelineState() const
{
	return m_pipelineState;
}
