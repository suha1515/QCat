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
	float4 pos :SV_Position;
};
VSOut main(float3 pos : a_Position, float2 tc : a_Texture)
{
	VSOut vso;

	matrix mat = mul(viewProj, transform);
	vso.pos = mul(mat, float4(pos, 1.0f));
	vso.tc = tc;
	return vso;
}