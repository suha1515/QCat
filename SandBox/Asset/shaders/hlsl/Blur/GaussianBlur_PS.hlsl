cbuffer Horizon : register(b0)
{
	bool horizontal;
}
Texture2D image;
SamplerState splr :register(s0);
const static float weight[5] = { 0.227027f,0.1945846f,0.1216216f,0.054054f,0.016216f };
float4 main(float2 tc : Texcoord) : SV_TARGET
{
	float width,height;
	image.GetDimensions(width, height);
	float2 textureSize = float2(1.0f / width, 1.0f / height);

	float3 result = image.Sample(splr, tc).rgb * weight[0];
	if (horizontal)
	{
		float2 texCoords;
		[unroll]
		for (int i = 0; i < 5; ++i)
		{
			result += image.Sample(splr, tc,int2(i,0)).rgb * weight[i];
			result += image.Sample(splr, tc,int2(-i,0)).rgb * weight[i];
		}
	}
	else
	{
		float2 texCoords;
		[unroll]
		for (int i = 0; i < 5; ++i)
		{
			result += image.Sample(splr, tc,int2(0,i)).rgb * weight[i];
			result += image.Sample(splr, tc,int2(0,-i)).rgb * weight[i];
		}
	}
	float4 color = float4(result, 1.0f);
	return color;
}