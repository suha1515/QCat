
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
	float4 pos :SV_Position;
};

VSOut main(float3 pos : a_Position)
{
	VSOut vso;
	
	matrix mat = mul(viewProj, transform);
	vso.pos = mul(mat, float4(pos, 1.0f));
	return vso;
}