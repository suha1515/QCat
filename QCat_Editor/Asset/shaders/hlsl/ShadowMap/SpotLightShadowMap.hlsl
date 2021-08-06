//Cascade Shadow Map applied
#type vertex
struct VSIn
{
	float3 pos : a_Position;
	float3 normal : a_Normal;
	float2 tc  : a_TexCoord;
};

cbuffer Transform : register(b0)
{
	matrix u_Transform;
	matrix u_InvTransform;
}
cbuffer LightTransform : register(b1)
{
	matrix lightTransform;
}
struct VSOut
{
	float2 tc		: Texcoords;
	float4 position : SV_POSITION;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	output.tc = input.tc;
	output.position = mul(lightTransform,mul(u_Transform, float4(input.pos, 1.0f)));
	return output;
}
#type pixel
struct PSIn
{
	float2 tc		: Texcoords;
	float4 position : SV_POSITION;
};
void PSMain(PSIn input)
{
}