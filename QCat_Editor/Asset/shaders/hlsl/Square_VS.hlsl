cbuffer viewProjection : register(b0)
{
	matrix u_ViewProjection;
}
cbuffer Transform : register(b1)
{
	matrix transform;
}
struct VSIn
{
	float3 pos		  : a_Position;
	float4 color      : a_Color;
	float2 tc		  : a_Texcoord;
	float texIndex	  : a_TexIndex;
	float tilingFactor: a_TilingFactor;
	int entityID	  : a_EntityID;
};
struct VSOut
{
	float2 tc: Texcoord;
	float4 color : Color;
	float texIndex : v_TexIndex;
	float tilingFactor : v_TilingFactor;
	int   entityID : v_EntityID;
	float4 pos :SV_Position;
};
VSOut main(VSIn input)
{
	VSOut vso;

	//matrix mat = mul(viewProj, transform);
	matrix mat = u_ViewProjection;
	vso.pos = mul(mat, float4(input.pos, 1.0f));
	vso.tc = input.tc;
	vso.color = input.color;
	vso.texIndex = input.texIndex;
	vso.tilingFactor = input.tilingFactor;
	vso.entityID = input.entityID;
	return vso;
}