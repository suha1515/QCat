#type compute

cbuffer fixed
{
	static const int blurRadius = 5;
};

Texture2D inputImage : register(t0);
SamplerState splr : register(s0);
RWTexture2D<float4> outputImage: register(u0);

groupshared float4 gCache[266];
const static float weights[11] = { 0.05f,0.05f,0.1f,0.1f,0.1f,0.2f,0.1f,0.1f,0.1f,0.05f,0.05f };

[numthreads(256, 1, 1)]
void CSMain(int3 groupindex :SV_GroupThreadID, int3 globalindex : SV_DispatchThreadID)
{
	float width, height;
	inputImage.GetDimensions(width, height);

	float2 imgSize = float2(width, height);
	float2 index = min(globalindex.xy, imgSize);
	float2 coord = index / imgSize;

	gCache[groupindex.x + 5] = inputImage.SampleLevel(splr, coord, 0);

	if (groupindex.x < 5)
	{
		int x = max(globalindex.x - 5, 0);
		coord.x = (x / imgSize.x);
		gCache[groupindex.x] = inputImage.SampleLevel(splr, coord, 0);
	}
	if (groupindex.x >= 251)
	{
		int x = min(globalindex.x + 5, imgSize.x);
		coord.x = (x / imgSize.x);
		gCache[groupindex.x + 10] = inputImage.SampleLevel(splr, coord, 0);
	}

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	[unroll]
	for (int i = -5; i <= 5; ++i)
	{
		int k = groupindex.x + 5 +i;
		blurColor += gCache[k] * weights[i + 5];
	}
	outputImage[globalindex.xy] = blurColor;

}
