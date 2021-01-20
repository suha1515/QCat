cbuffer lightColor : register(b0)
{
	float3 color;
}

Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(float2 tc: Texcoord, float3 normal : Normal) : SV_TARGET
{
	return tex.Sample(splr, tc)*float4(color,1.0f);
}