Texture2D g_texture : register(t0);

SamplerState g_sampler : register(s0);

cbuffer cbGameObject : register(b0)
{
	matrix g_worldMatrix;
	float g_layer;
	float g_alpha;
	bool g_isFliped;
	float dummy;
}

cbuffer cbCamera : register(b1)
{
	matrix g_viewMatrix;
	matrix g_projMatrix;
}

cbuffer cbTexture : register(b2)
{
	uint2 g_size;
	float2 _dummy;
}