#type vertex
cbuffer u_ViewProjection : register(b0)
{
	matrix u_ViewProjection;
}
cbuffer u_Transform : register(b1)
{
	matrix u_Transform;
	matrix u_invTransform;
}
struct VSOut
{
	float2 tc: Texcoord;
	float3 normal :  Normal;
	float3 fragPos : FragPos;
	float4 pos :SV_Position;

};
VSOut VSMain(float3 pos : a_Position, float3 normal : a_Normal, float2 tc : a_Texcoord)
{
	VSOut vso;

	matrix mat = mul(u_ViewProjection, u_Transform);
	vso.pos = mul(mat, float4(pos, 1.0f));
	vso.tc = tc;
	vso.normal = mul((float3x3)transpose(u_invTransform), normal);
	vso.fragPos = (float3)mul(u_Transform, float4(pos, 1.0f));
	return vso;
}
#type pixel
cbuffer u_Color : register(b0)
{
	float4 u_color;
}

float4 PSMain() : SV_TARGET
{
	return u_color;
}