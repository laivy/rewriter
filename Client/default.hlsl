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
	float2 uv : TEXCOORD;
};

GS_INPUT VS(VS_INPUT input)
{
	GS_INPUT output = (GS_INPUT) 0;
	return output;
}

[maxvertexcount(4)]
void GS(point GS_INPUT input[1], inout TriangleStream<PS_INPUT> triangleStream)
{
	float hx = g_size.x / 2.0f;
	float hy = g_size.y / 2.0f;
	
	float4 position[4] =
	{
		float4(input[0].positionW.x - hx, input[0].positionW.y - hy, g_layer, 1.0f),
		float4(input[0].positionW.x - hx, input[0].positionW.y + hy, g_layer, 1.0f),
		float4(input[0].positionW.x + hx, input[0].positionW.y - hy, g_layer, 1.0f),
		float4(input[0].positionW.x + hx, input[0].positionW.y + hy, g_layer, 1.0f)
	};
	
	float2 uv[4] =
	{
		float2(0.0f + g_isFliped, 1.0f),
		float2(0.0f + g_isFliped, 0.0f),
		float2(1.0f - g_isFliped, 1.0f),
		float2(1.0f - g_isFliped, 0.0f)
	};

	PS_INPUT output;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		output.positionW = mul(position[i], g_worldMatrix);
		output.positionH = mul(mul(output.positionW, g_viewMatrix), g_projMatrix);
		output.uv = uv[i];
		triangleStream.Append(output);
	}
}

float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 color = g_texture.Sample(g_sampler, input.uv);
	color.a *= g_alpha;
	return color;
}