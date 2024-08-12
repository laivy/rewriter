#pragma once

class Shader
{
public:
	enum class Type
	{
		DEFAULT, LINE
	};

public:
	Shader();
	~Shader() = default;

	ID3D12PipelineState* GetPipelineState() const;

protected:
	ComPtr<ID3D12PipelineState>	m_pipelineState;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
};

class LineShader : public Shader
{
public:
	LineShader();
	~LineShader() = default;
};