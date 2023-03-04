Texture2D g_texture : register(t0);

SamplerState g_sampler : register(s0);

cbuffer cbGameObject : register(b0)
{
	matrix g_worldMatrix;
}

cbuffer cbCamera : register(b1)
{
	matrix g_viewMatrix;
	matrix g_projMatrix;
}

cbuffer cbImage : register(b2)
{
	float2 g_size;
	float2 _dummy;
}