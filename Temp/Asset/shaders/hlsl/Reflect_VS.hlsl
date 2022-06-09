cbuffer Transform : register(b0)
{
	matrix u_ViewProjection;
	matrix u_Transform;
	matrix u_invTransform;
}

struct VSOut
{
	float3 normal   : Normal;
	float3 worldPos : PositionW;
	float4 position : SV_POSITION;
};

VSOut main( float3 pos : a_Position,float3 normal : a_Normal)
{
	VSOut vso;
	float3x3 normalMatrix = (float3x3)transpose(u_invTransform);
	vso.normal   = mul(normalMatrix,normal);
	vso.worldPos = mul(u_Transform, float4(pos, 1.0f));
	vso.position = mul(u_ViewProjection, float4(vso.worldPos, 1.0f));

	return vso;
}