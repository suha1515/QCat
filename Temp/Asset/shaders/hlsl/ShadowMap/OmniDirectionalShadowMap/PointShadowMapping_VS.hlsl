cbuffer Transform : register(b0)
{
	matrix u_Transform;
}
struct VSOut
{
	float2 tc		: TexCoords;
	float4 position : SV_POSITION;
};

VSOut main( float3 pos : a_Position,float3 normal : a_Normal,float2 tc : a_TexCoord)
{
	VSOut vso;
	vso.tc = tc;
	vso.position = mul(u_Transform, float4(pos, 1.0f));

	return vso;
}