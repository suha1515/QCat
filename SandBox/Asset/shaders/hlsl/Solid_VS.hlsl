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
	float4 color : Color;
	float4 pos :SV_Position;
	float tilingFactor : v_TilingFactor;
};
VSOut main(float3 pos : a_Position, float4 color : a_Color, float2 tc : a_Texcoord, float tilingFactor : a_TilingFactor)
{
	VSOut vso;

	matrix mat = mul(viewProj, transform);
	vso.pos = mul(mat, float4(pos, 1.0f));
	vso.tc = tc;
	vso.color = color;
	vso.tilingFactor = tilingFactor;
	return vso;
}