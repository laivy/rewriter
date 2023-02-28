#include "Common.hlsli"

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 positionH : SV_POSITION;
	float4 positionW : POSITION;
	float4 color : COLOR;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.positionW = float4(input.position, 1.0f);
	output.positionH = mul(mul(output.positionW, g_viewMatrix), g_projMatrix);
	output.color = input.color;
	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
	return input.color;
}