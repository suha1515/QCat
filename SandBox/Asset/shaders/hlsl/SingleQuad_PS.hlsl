
Texture2D screenTex;
SamplerState splr :register(s0);
float4 main(float2 tc : Texcoord) : SV_TARGET
{
	return screenTex.Sample(splr,tc);
}