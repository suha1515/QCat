
Texture2D screenTex;
SamplerState splr :register(s0);
float4 main(float2 tc : Texcoord) : SV_TARGET
{
	float depthValue = screenTex.Sample(splr,tc).r;
	float4 color = float4(depthValue,depthValue,depthValue, 1.0f);
	return color;
}