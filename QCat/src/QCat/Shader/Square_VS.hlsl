cbuffer TransformCbuf : register(b0)
{
	matrix viewProj;
}
float4 main( float3 pos : Position ) : SV_POSITION
{
	return mul(viewProj,float4(pos,1.0f));
}