
Texture2D gPosition;
Texture2D gNormal;
Texture2D texNoise;

SamplerState posTex :register(s0);
SamplerState normalTex   :register(s1);
SamplerState noiseTex  :register(s2);

cbuffer Samples : register(b0)
{
	float3 samples[128];
	float2 screenSize;
	float radius;
	float bias;
	matrix Projection;
}

const static int kernelSize = 128;


float main(float2 tc : Texcoord) : SV_TARGET
{
	float2 noiseScale = float2(screenSize.x / 4.0f,screenSize.y / 4.0f);

	float3 fragPos = gPosition.Sample(posTex, tc).rgb;
	float3 normal = normalize(gNormal.Sample(normalTex, tc).rgb);
	float3 randomVec = normalize(texNoise.Sample(noiseTex, tc * noiseScale).xyz);

	float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	float3 bitangent = cross(normal, tangent);

	float3x3 TBN = { tangent, bitangent, normal};
	
	//TBN = transpose(TBN);
	float occlusion = 0.0f;
	for (int i = 0; i < kernelSize; ++i)
	{
		float3 samplePos = mul(samples[i], TBN);
		samplePos = fragPos + samplePos * radius;

		float4 offset = float4(samplePos, 1.0f);
		offset = mul(Projection, offset);
		offset.xy = (offset.xy/ offset.w);
		offset.xy = (offset.xy * float2(0.5f,-0.5f))+ 0.5f;

		float sampleDepth = gPosition.Sample(posTex, offset.xy).z;

		float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth <= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;
	}
	occlusion = 1.0f - (occlusion / kernelSize);

	return occlusion;
}