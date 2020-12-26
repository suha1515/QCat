
struct dd
{
	float4 a;
};
cbuffer u_ViewProjection : register(b0)
{
	matrix viewProj;
	float4 a;
	float3 b;
	int2 c;
	float d;
	float2 e;
}
cbuffer u_Transform : register(b1)
{
	matrix transform;
}
struct VSOut
{
	float4 pos :SV_Position;
};

VSOut main(float3 pos : Position)
{
	VSOut vso;
	
	matrix mat = mul(viewProj, transform);
	vso.pos = mul(mat, float4(pos, 1.0f));
	return vso;
}