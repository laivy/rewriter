export module rewriter.library.graphics.direct3d:pipeline_state;

export namespace Graphics::D3D
{
	enum class PipelineStateType
	{
		Model,
	};

	void SetPipelineState(PipelineStateType type);
}
