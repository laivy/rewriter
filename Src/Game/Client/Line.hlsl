#include "Common.hlsli"

struct VS_INPUT
{
	float4 position : POSITION;
};

struct GS_INPUT
{
	float4 positionW : POSITION;
};

struct PS_INPUT
{
	float4 positionH : SV_POSITION;
	float4 positionW : POSITION;
};

[maxvertexcount(2)]
void GS(point GS_INPUT input[1], inout LineStream<PS_INPUT> lineStream)
{
	float4 position[2] =
	{
		float4(g_point1, g_layer, 1.0f),
		float4(g_point2, g_layer, 1.0f)
	};

	PS_INPUT output;
	[unroll]
	for (int i = 0; i < 2; ++i)
	{
		output.positionW = position[i]; // 이미 월드 좌표계임
		output.positionH = mul(mul(output.positionW, g_viewMatrix), g_projMatrix);
		lineStream.Append(output);
	}
}

float4 PS(PS_INPUT input) : SV_TARGET
{
	return g_lineColor;
}