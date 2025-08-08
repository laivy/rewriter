#include "Constants.hlsli"

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	//output.positionH = mul(mul(mul(input.position, g_world), g_view), g_proj);
	output.positionH = mul(mul(input.position, g_view), g_proj);
	return output;
}
