cbuffer color : register(b0)
{
	float3 color;
}

float4 main() : SV_TARGET
{
	return float4(color.r, color.g, color.b, 1.0f);
}