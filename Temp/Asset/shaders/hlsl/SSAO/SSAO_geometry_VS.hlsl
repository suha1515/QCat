
cbuffer u_ViewProjection : register(b0)
{
	matrix view;
	matrix invView;
	matrix proj;
}
cbuffer u_Transform : register(b1)
{
	matrix u_Transform;
	matrix u_invTransform;
	bool flip;
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
	float3 viewfragPos	: FragPos;
	float2 tc			: Texcoord;
	float3 viewnormal	:  Normal;
	float3x3 tbn		: TBN;
	float4 pos :SV_Position;
};
VSOut main(VSIn input)
{
	VSOut vso;

	matrix viewmat = mul(view, u_Transform);
	matrix viewprojmat = mul(proj, viewmat);
	float3x3 normalMat = (float3x3)transpose(mul(u_invTransform, invView));
	vso.pos = mul(viewprojmat, float4(input.pos, 1.0f));
	vso.tc = input.tc;
	vso.viewnormal = mul(normalMat, (flip? -input.normal : input.normal));
	vso.viewfragPos = (float3)mul(viewmat, float4(input.pos, 1.0f));

	float3 T = normalize(mul(normalMat, input.tan));
	float3 B = normalize(mul(normalMat, input.bitan));
	float3 N = normalize(mul(normalMat, input.normal));

	float3x3 TBN = float3x3( T,B,N );
	vso.tbn = transpose(TBN);

	return vso;
}