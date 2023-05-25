#include "Stdafx.h"
#include "Shader.h"
#include "GameApp.h"

Shader::Shader()
{
	// 레이아웃 설정
	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// 셰이더 컴파일
	ComPtr<ID3DBlob> vertexShader, geometryShader, pixelShader, error;
#ifdef _DEBUG
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	HRESULT hr{ E_FAIL };
	hr = D3DCompileFromFile(L"Default.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_1", compileFlags, 0, &vertexShader, &error);
	hr |= D3DCompileFromFile(L"Default.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_5_1", compileFlags, 0, &geometryShader, &error);
	hr |= D3DCompileFromFile(L"Default.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_1", compileFlags, 0, &pixelShader, &error);
#ifdef _DEBUG
	if (FAILED(hr))
	{
		OutputDebugStringA(reinterpret_cast<char*>(error->GetBufferPointer()));
		assert(false);
	}
#endif

	CD3DX12_BLEND_DESC blendDesc{ D3D12_DEFAULT };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// PSO 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { m_inputLayout.data(), static_cast<UINT>(m_inputLayout.size()) };
	psoDesc.pRootSignature = GameApp::GetInstance()->GetRootSignature();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE{ vertexShader.Get() };
	psoDesc.GS = CD3DX12_SHADER_BYTECODE{ geometryShader.Get() };
	psoDesc.PS = CD3DX12_SHADER_BYTECODE{ pixelShader.Get() };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
	psoDesc.BlendState = blendDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	DX::ThrowIfFailed(GameApp::GetInstance()->GetD3DDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

ID3D12PipelineState* Shader::GetPipelineState() const
{
	return m_pipelineState.Get();
}
