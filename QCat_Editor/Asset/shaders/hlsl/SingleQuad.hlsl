#type vertex
struct VSOut
{
	float2 tc : Texcoord;
	float4 position : SV_POSITION;
};

VSOut VSMain(float2 pos : a_Position, float2 tc : a_TexCoords)
{
	VSOut vso;
	vso.position = float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.tc = tc;
	return vso;
}
#type pixel
Texture2D screenTex;
SamplerState splr :register(s0);
float4 PSMain(float2 tc : Texcoord) : SV_TARGET
{
	float3 color = screenTex.Sample(splr, tc).rgb;
	return float4(color,1.0f);
}