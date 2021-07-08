#type vertex
struct VSOut
{
	float2 tc : Texcoords;
	float4 position : SV_POSITION;
};

VSOut VSMain(float2 pos : a_Position,float2 tc : a_TexCoords)
{
	VSOut vso;
	vso.position = float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.tc = tc;
	return vso;
}
#type pixel
Texture2D screenTex : register(t0);
SamplerState splr :register(s0);

struct PSIn
{
	float2 tc		: Texcoords;
	float4 position : SV_POSITION;
};
float4 PSMain(PSIn input) : SV_TARGET
{
	float depthValue = screenTex.Sample(splr,input.tc).r;
	float4 color = float4(depthValue,depthValue,depthValue, 1.0f);
	return color;
}