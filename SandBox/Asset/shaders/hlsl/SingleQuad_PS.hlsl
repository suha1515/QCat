
Texture2D screenTex;
SamplerState splr :register(s0);
const float offsetX =1.0f/1600.0f;
const float offsetY = 1.0f / 900.0f;
float4 main(float2 tc : Texcoord) : SV_TARGET
{
	float4 color;
	float r = screenTex.Sample(splr, tc).r;
	color = float4(r, r, r, 1.0f);
	return color;
}