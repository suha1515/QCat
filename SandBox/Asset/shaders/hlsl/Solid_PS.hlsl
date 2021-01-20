
Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(float2 tc: Texcoord, float4 color : Color, float tilingFactor : v_TilingFactor) : SV_TARGET
{
	return tex.Sample(splr, tc * tilingFactor) * color;
}