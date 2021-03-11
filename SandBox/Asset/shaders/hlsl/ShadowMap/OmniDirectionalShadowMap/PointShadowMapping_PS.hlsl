cbuffer lightInfo : register(b0)
{
	float3 lightPos;
	float  far_plane;
}

struct PSIn
{
	float2 tc		: TexCoords;
	float4 FragPos  : FragPos;
	float4 pos		: SV_POSITION;
};

struct PS_OUT
{
	float depth : SV_Depth;
};

Texture2D diffuseTex;
SamplerState splr :register(s0);

void main(PSIn input)
{
	//PS_OUT psOut;
	float4 texColor = diffuseTex.Sample(splr,input.tc);
	if (texColor.a < 0.1f)
		discard;

	//float lightDistance = length(input.FragPos.xyz - lightPos);
	//lightDistance = lightDistance / far_plane;

	//psOut.depth = lightDistance;
	//return psOut;
}