#pragma once

namespace Graphics::D3D
{
	enum class PipelineStateType
	{
		Model,
	};

	void SetPipelineState(PipelineStateType type);
}
