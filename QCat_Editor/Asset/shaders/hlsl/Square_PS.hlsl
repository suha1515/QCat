
struct PS_OUT
{
	float4 color : SV_TARGET0;
	int id : SV_TARGET1;
};

Texture2D tex[32] : register(t0);
SamplerState splr : register(s0);

PS_OUT main(float2 tc: Texcoord, float4 color : Color, float texIndex : v_TexIndex, float tilingFactor : v_TilingFactor,int entityID : v_EntityID)
{
	int index = (int)texIndex;
	PS_OUT output;
	[unroll]
	for (int i = 0; i < 32; ++i)
	{
		if (index == 0)
		{
			output.color = tex[0].Sample(splr, tc * tilingFactor) * color;
			output.id = entityID;
			return output;
		}
		if (i == index)
		{
			output.color =  tex[i].Sample(splr, tc * tilingFactor) * color;
			output.id = entityID;
			return output;
		}	
	}
	output.color = tex[0].Sample(splr, tc * tilingFactor) * color;
	output.id = entityID;
	return output;
}