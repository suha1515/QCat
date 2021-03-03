cbuffer u_ViewProjection : register(b0)
{
	matrix u_ViewProjection;
	float3 viewPosition;
}
cbuffer u_Transform : register(b1)
{
	matrix u_Transform;
	matrix u_invTransform;
	matrix lightSpaceMatrix;
}
cbuffer u_LightInfo : register(b2)
{
	float3 lightPosition;
}
struct VSOut
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float4 fragPosLightSpace : FragPosLightSpace;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
};
VSOut main(float3 pos : a_Position, float3 normal : a_Normal, float2 tc : a_Texcoord, float3 tan : a_Tangent, float3 bitan : a_BiTangent)
{
	VSOut vso;

	matrix viewprojMat = mul(u_ViewProjection, u_Transform);
	float3x3 normalMat = (float3x3)transpose(u_invTransform);
	vso.pos = mul(viewprojMat, float4(pos, 1.0f));
	vso.tc = tc;
	vso.normal = mul(normalMat, normal);

	float3 T = normalize(mul(normalMat, tan));
	float3 B = normalize(mul(normalMat, bitan));
	float3 N = normalize(mul(normalMat, normal));

	//T = normalize(T - dot(T, N) * N);

	float3x3 mat = { T,B,N };
	vso.TBN = mat;

	//TBN = transpose(TBN);
	
	float3 FragPos = (float3)mul(u_Transform, float4(pos, 1.0f));
	vso.fragPos = FragPos;
	vso.viewPosition = viewPosition;
	vso.fragPosLightSpace = mul(lightSpaceMatrix, float4(FragPos, 1.0f));
	vso.fragPosLightSpace= vso.fragPosLightSpace *float4(0.5f, 0.5f, 1.0f, 1.0f) + float4(0.5f, 0.5f, 0.0f, 0.0f)* vso.fragPosLightSpace.w;
	return vso;
}