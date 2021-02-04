cbuffer u_Color : register(b0)
{
	float4 color;
}

float4 main() : SV_TARGET
{
	return float4(1.0f,1.0f,1.0f,1.0f);
}