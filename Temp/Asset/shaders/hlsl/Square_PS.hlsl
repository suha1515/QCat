
Texture2D tex[32] : register(t0);
SamplerState splr : register(s0);

float4 main(float2 tc: Texcoord, float4 color : Color, float texIndex : v_TexIndex, float tilingFactor : v_TilingFactor) : SV_TARGET
{
	int index = (int)texIndex;
	[unroll]
	for (int i = 0; i < 32; ++i)
	{
		if (index == 0)
		{
			return tex[0].Sample(splr, tc * tilingFactor) * color;
		}
		if (i == index)
		{
			return tex[i].Sample(splr, tc * tilingFactor) * color;
		}	
	}
	return tex[0].Sample(splr, tc * tilingFactor) * color;
}