cbuffer u_ViewProjection : register(b0)
{
	matrix viewProj;
}
cbuffer u_Transform : register(b1)
{
	matrix transform;
}
struct VSOut
{
	float2 tc: Texcoord;
	float3 normal :  Normal;
	float4 pos :SV_Position;

};
VSOut main(float3 pos : a_Position, float3 normal : a_Normal, float2 tc : a_Texcoord)
{
	VSOut vso;

	matrix mat = mul(viewProj, transform);
	vso.pos = mul(mat, float4(pos, 1.0f));
	vso.tc = tc;
	vso.normal = normal;
	return vso;
}