
cbuffer u_ViewProjection : register(b0)
{
	matrix u_ViewProjection;
}
cbuffer u_Transform : register(b1)
{
	matrix u_Transform;
	matrix u_invTransform;
}
struct VSIn
{
	float3 pos		: a_Position;
	float3 normal	: a_Normal;
	float2 tc		: a_TexCoord;
	float3 tan		: a_Tangent;
	float3 bitan	: a_BiTangent;
};
struct VSOut
{
	float3 fragPos	: FragPos;
	float2 tc		: Texcoord;
	float3 normal	:  Normal;
	float3x3 tbn    : TBN;
	float4 pos :SV_Position;

};
VSOut main(VSIn input)
{
	VSOut vso;

	matrix mat = mul(u_ViewProjection, u_Transform);
	float3x3 normalMat = (float3x3)transpose(u_invTransform);
	vso.pos = mul(mat, float4(input.pos, 1.0f));
	vso.tc = input.tc;
	vso.normal = mul(normalMat, input.normal);
	vso.fragPos = (float3)mul(u_Transform, float4(input.pos, 1.0f));

	float3 T = normalize(mul(normalMat, input.tan));
	float3 B = normalize(mul(normalMat, input.bitan));
	float3 N = normalize(mul(normalMat, input.normal));

	float3x3 TBN = { T,B,N };
	vso.tbn = TBN;

	return vso;
}