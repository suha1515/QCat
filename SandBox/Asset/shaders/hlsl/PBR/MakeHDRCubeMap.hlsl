#type vertex
cbuffer trnasform  : register(b0)
{
	matrix u_Projection;
	matrix u_View;
}
struct VSOut
{
	float3 localPos			 : Local;
	float4 pos :SV_Position;
};

struct VSIn
{
	float3 pos		: a_Position;
};
VSOut VSMain(VSIn Input)
{
	VSOut vso;

	vso.localPos = Input.pos;
	//matrix rotView = float4x4(float3x3(u_View));
	vso.pos = mul(u_Projection, mul(u_View, float4(Input.pos, 1.0f)));
	
	return vso;
}
#type pixel
struct PSIn
{
	float3 localPos			 : Local;
	float4 pos :SV_Position;
};
struct PS_OUT
{
	float4 color :SV_TARGET0;
};

TextureCube environmentMap;
SamplerState splr : register(s0);

PS_OUT PSMain(PSIn input)
{
	PS_OUT output;
	
	float3 dir = input.localPos;
	float3 envColor = environmentMap.Sample(splr, dir).rgb;

	output.color = float4(envColor, 1.0f);
	//output.color = float4(1.0f,1.0f,1.0f,1.0f);
	return output;
}