#include "Stdafx.h"
#include "Global.h"
#include "PipelineState.h"
#include "Shader/Model.vs.h"
#include "Shader/Model.ps.h"

namespace Graphics::D3D
{
	std::map<PipelineStateType, ComPtr<ID3D12PipelineState>> g_pipelineStates;

	class PipelineStateBuilder
	{
	public:
		PipelineStateBuilder() :
			m_desc{}
		{
			m_desc.pRootSignature = g_rootSignature.Get();
			m_desc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
			m_desc.SampleMask = UINT_MAX;
			m_desc.RasterizerState = CD3DX12_RASTERIZER_DESC{ D3D12_DEFAULT };
			m_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC{ D3D12_DEFAULT };
			m_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			m_desc.NumRenderTargets = 1;
			m_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			m_desc.SampleDesc.Count = 1;
		}

		~PipelineStateBuilder() = default;

		PipelineStateBuilder& VertexShader(std::span<const BYTE> bytecode)
		{
			m_desc.VS = CD3DX12_SHADER_BYTECODE{ bytecode.data(), bytecode.size() };
			return *this;
		}

		PipelineStateBuilder& PixelShader(std::span<const BYTE> bytecode)
		{
			m_desc.PS = CD3DX12_SHADER_BYTECODE{ bytecode.data(), bytecode.size() };
			return *this;
		}

		PipelineStateBuilder& InputLayout(std::initializer_list<std::tuple<std::string_view, DXGI_FORMAT>> elements)
		{
			m_inputElementDescs.clear();
			m_inputElementDescs.reserve(elements.size());

			UINT alignedByteOffset{ 0 };
			for (const auto& [name, format] : elements)
			{
				D3D12_INPUT_ELEMENT_DESC desc{};
				desc.SemanticName = name.data();
				desc.SemanticIndex = 0;
				desc.Format = format;
				desc.InputSlot = 0;
				desc.AlignedByteOffset = alignedByteOffset;
				desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				desc.InstanceDataStepRate = 0;
				m_inputElementDescs.push_back(desc);

				switch (format)
				{
				case DXGI_FORMAT_R32G32B32_FLOAT:
					alignedByteOffset += 3 * sizeof(float);
					break;
				default:
					assert(false && "UNSUPPORTED FORMAT");
					break;
				}
			}

			m_desc.InputLayout = D3D12_INPUT_LAYOUT_DESC{ m_inputElementDescs.data(), static_cast<UINT>(m_inputElementDescs.size()) };
			return *this;
		}

		ComPtr<ID3D12PipelineState> Build() const
		{
			ComPtr<ID3D12PipelineState> pso;
			if (FAILED(g_d3dDevice->CreateGraphicsPipelineState(&m_desc, IID_PPV_ARGS(&pso))))
				return nullptr;
			return pso;
		}

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
	};

	void SetPipelineState(PipelineStateType type)
	{
		if (g_pipelineStates.contains(type))
		{
			g_commandList->SetPipelineState(g_pipelineStates[type].Get());
			return;
		}

		switch (type)
		{
		case PipelineStateType::Model:
		{
			auto pso = PipelineStateBuilder{}
				.VertexShader(ModelVertexShader)
				.PixelShader(ModelPixelShader)
				.InputLayout
				({
					{ "POSITION", DXGI_FORMAT_R32G32B32_FLOAT }
				})
				.Build();

			g_pipelineStates.emplace(type, pso);
			break;
		}
		default:
			assert(false && "INVALID PIPELINE STATE TYPE");
			break;
		}
	}
}
