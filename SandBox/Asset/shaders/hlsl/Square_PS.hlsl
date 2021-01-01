cbuffer u_Color : register(b0)
{
	float4 u_color;
}
cbuffer u_TilingFactor : register(b1)
{
	float factor;
}
Texture2D tex : register(t0);
SamplerState splr : register(s0);
float4 main(float2 tc: Texcoord, float4 color : Color) : SV_TARGET
{
	//return tex.Sample(splr,tc * factor)* color;
	return color;
}