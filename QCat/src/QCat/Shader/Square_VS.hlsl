cbuffer TransformCbuf : register(b0)
{
	matrix viewProj;
}
cbuffer transform : register(b1)
{
	matrix transform;
}

float4 main(float3 pos : Position) : SV_POSITION
{
	matrix mat = mul(viewProj,transform);
	return mul(mat,float4(pos,1.0f));
}