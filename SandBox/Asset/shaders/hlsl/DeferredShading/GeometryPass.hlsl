
struct PSIn
{
	float2 tc: Texcoord;
	float3 normal :  Normal;
	float3 fragPos : FragPos;
	float4 pos :SV_Position;
};

struct PS_OUT
{
	float3 gPosition : SV_TARGET0;
	float3 gNormal   : SV_TARGET1;
	float4 gAlbedoSpec: SV_TARGET2;
};

Texture2D texture_diffuse;
Texture2D texture_specular;

SamplerState splr : register(s0);

PS_OUT main(PSIn input)
{
	PS_OUT output;

	output.gPosition = input.fragPos;
	output.gNormal = input.normal;
	output.gAlbedoSpec.rgb = texture_diffuse.Sample(splr, input.tc).rgb;
	output.gAlbedoSpec.a = texture_specular.Sample(splr, input.tc).r;

	return output;
}