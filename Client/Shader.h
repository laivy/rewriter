#pragma once

class Shader
{
public:
	enum class Type
	{
		DEFAULT
	};

public:
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~Shader() = default;

	ComPtr<ID3D12PipelineState> GetPipelineState() const;

private:
	ComPtr<ID3D12PipelineState>	m_pipelineState;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
};