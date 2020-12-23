cbuffer color : register(b0)
{
	float4 color;
}

Texture2D tex : register(t0);
SamplerState splr : register(s0);
float4 main() : SV_TARGET
{
	return color;
}