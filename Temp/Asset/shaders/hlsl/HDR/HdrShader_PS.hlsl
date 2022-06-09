cbuffer exposure : register (b0)
{
	float exposure;
}
Texture2D screenTex;
SamplerState splr :register(s0);
float4 main(float2 tc : Texcoord) : SV_TARGET
{
	const float gamma = 2.2f;
	float3 hdrColor = screenTex.Sample(splr, tc).rgb;

	// exposure tone Mapping
	float3 mapped = float3(1.0f, 1.0f, 1.0f) - exp(-hdrColor * exposure);
	// gamma Correction
	mapped = pow(mapped,1.0f/gamma);
	
	float4 color;
	color = float4(mapped, 1.0f);

	return color;
}