struct PSIn
{
	float3 normal   : Normal;
	float3 worldPos : PositionW;
	float4 position : SV_POSITION;
};

cbuffer viewPos : register(b0)
{
	float3 viewPosition;
}

TextureCube skybox;
SamplerState splr :register(s0);

float4 main(PSIn input) : SV_TARGET
{
	float3 I = normalize(input.worldPos - viewPosition);
	float ratio = 1.00f / 1.52f;
	//float3 R = refract(I, normalize(input.normal), ratio);
	float3 R = reflect(I, normalize(input.normal));
	float4 color = float4(skybox.Sample(splr, R).rgb, 1.0f);

	return color;
}