cbuffer transform : register (b0)
{
	matrix u_ViewProjection;
	matrix u_Transform;
	matrix u_invTransform;
}
struct VSIn
{
	float3 pos    : a_Position;
	float3 normal : a_Normal;
};
struct VSOut
{
	float4 position : Position;
	float3 normal : Normal;
};
VSOut main( VSIn input )
{
	VSOut vout;
	float3x3 normalMatrix = (float3x3)transpose(mul(u_ViewProjection, u_invTransform));

	vout.normal = normalize(mul(normalMatrix, input.normal));	
	vout.position = mul(mul(u_ViewProjection, u_Transform), float4(input.pos, 1.0f));
	return vout;
}