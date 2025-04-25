
cbuffer Object : register(b0)
{
	matrix g_world;
};

cbuffer Camera : register(b1)
{
	matrix g_view;
	matrix g_proj;
};

struct VertexShaderInput
{
	float4 position : POSITION;
};

struct PixelShaderInput
{
	float4 positionH : SV_POSITION;
};
