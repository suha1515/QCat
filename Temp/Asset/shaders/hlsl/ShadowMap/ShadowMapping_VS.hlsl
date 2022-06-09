cbuffer Transform : register(b0)
{
	matrix u_ViewProjection;
	matrix u_Transform;
}
struct VSOut
{
	float2 tc		: TexCooords;
	float4 position : SV_POSITION;
};

VSOut main( float3 pos : a_Position,float3 normal : a_Normal,float2 tc : a_TexCoord)
{
	VSOut vso;
	vso.tc = tc;
	matrix mat = mul(u_ViewProjection, u_Transform);
	vso.position = mul(mat, float4(pos, 1.0f));

	return vso;
}