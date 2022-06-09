
struct PSIn
{
	float3 fragPos	: FragPos;
	float2 tc		: Texcoord;
	float3 normal	:  Normal;
	float3x3 tbn    : TBN;
	float4 pos :SV_Position;

};

struct PS_OUT
{
	float3 gPosition : SV_TARGET0;
	float3 gNormal   : SV_TARGET1;
	float4 gAlbedoSpec: SV_TARGET2;
};

struct Material
{
	float3 diffuse;
	bool normalMap;
};

cbuffer Material : register(b0)
{
	Material material;
	bool flip;
}

Texture2D texture_diffuse :register(t0);
Texture2D texture_specular:register(t1);
Texture2D texture_normal  :register(t2);

SamplerState diffuseSplr  : register(s0);
SamplerState specularSplr : register(s1);
SamplerState normalSplr   : register(s2);


PS_OUT main(PSIn input)
{
	PS_OUT output;

	output.gPosition = input.fragPos;
	float3 norm;
	if (material.normalMap)
	{
		norm = texture_normal.Sample(normalSplr, input.tc).rgb;
		norm = norm * 2.0f - 1.0f;
		norm = normalize(mul(input.tbn, norm));
	}
	else
		norm = normalize(input.normal);

	if (flip)
		norm = -norm;

	output.gNormal = norm;
	output.gAlbedoSpec.rgb = texture_diffuse.Sample(diffuseSplr, input.tc).rgb * material.diffuse;
	output.gAlbedoSpec.a = texture_specular.Sample(specularSplr, input.tc).r;

	return output;
}