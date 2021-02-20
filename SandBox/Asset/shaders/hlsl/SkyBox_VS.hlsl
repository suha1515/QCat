cbuffer u_ViewProjection : register(b0)
{
	matrix u_ViewProjection;
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
	vso.pos = mul(u_ViewProjection, float4(pos, 1.0f));
	vso.pos.z = vso.pos.w;
	return vso;
}