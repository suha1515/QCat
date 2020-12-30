cbuffer u_Color : register(b0)
{
	float3 color;
}
Texture2D tex : register(t0);
SamplerState splr : register(s0);
float4 main(float2 tc: Texcoord) : SV_TARGET
{
	return tex.Sample(splr,tc);
}