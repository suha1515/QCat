cbuffer TransformCbuf : register(b0)
{
	matrix viewProj;
}
cbuffer transform : register(b1)
{
	matrix transform;
}
struct VSOut
{
	float2 tc: Texcoord;
	float4 pos :SV_Position;
};

VSOut main(float3 pos : Position,float2 tc : Texcoord)
{
	VSOut vso;
	
	matrix mat = mul(viewProj,transform);
	vso.pos = mul(mat, float4(pos, 1.0f));
	vso.tc = tc;
	return vso;
}