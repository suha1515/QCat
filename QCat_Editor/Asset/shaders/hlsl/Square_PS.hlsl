struct PS_OUT
{
	float4 color : SV_TARGET0;
};

Texture2D tex[32] : register(t0);
SamplerState splr : register(s0);

PS_OUT main(float2 tc: Texcoord, float4 color : Color, float texIndex : v_TexIndex, float tilingFactor : v_TilingFactor)
{
	int index = (int)texIndex;
	PS_OUT output;
	[unroll]
	for (int i = 0; i < 32; ++i)
	{
		if (index == 0)
		{
			output.color = tex[0].Sample(splr, tc * tilingFactor) * color;
			return output;
		}
		if (i == index)
		{
			output.color =  tex[i].Sample(splr, tc * tilingFactor) * color;
			return output;
		}	
	}
	
	output.color = tex[0].Sample(splr, tc * tilingFactor) * color;
	return output;
}