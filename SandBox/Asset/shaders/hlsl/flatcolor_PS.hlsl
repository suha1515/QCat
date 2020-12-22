cbuffer color : register(b0)
{
	float3 color;
}

Texture2D tex : register(t0);
SamplerState splr : register(s0);
float4 main(float2 tc : Texcoord) : SV_TARGET
{
	return float4(tex.Sample(splr,tc));
}