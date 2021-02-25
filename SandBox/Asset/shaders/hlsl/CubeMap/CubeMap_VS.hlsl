cbuffer u_ViewProjection : register(b0)
{
	matrix u_ViewProjection;
	matrix u_Transform;
}

struct VSOut
{
	float3 tc			   : Texcoord;
	float4 pos			   :SV_Position;
};
VSOut main(float3 pos : a_Position)
{
	VSOut vso;

	vso.tc = pos;
	vso.pos = mul(mul(u_ViewProjection,u_Transform), float4(pos, 1.0f));
	return vso;
}