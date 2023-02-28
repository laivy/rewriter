Texture2D g_texture1 : register(t0);

SamplerState g_sampler : register(s0);

cbuffer cbCamera : register(b0)
{
	matrix g_viewMatrix;
	matrix g_projMatrix;
}