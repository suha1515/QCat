cbuffer u_Color : register(b0)
{
	float4 u_color;
}

float4 main() : SV_TARGET
{
	return u_color;
}