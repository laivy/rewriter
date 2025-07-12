export module Library.Graphics.D3D:PipelineState;

export namespace Graphics::D3D
{
	enum class PipelineStateType
	{
		Model,
	};

	void SetPipelineState(PipelineStateType type);
}
