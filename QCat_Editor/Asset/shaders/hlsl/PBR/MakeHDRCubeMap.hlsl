#type vertex
cbuffer trnasform  : register(b0)
{
	matrix u_Projection;
	matrix u_View;
	float3 u_viewPosition;
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
	float4x4 rotView = u_View;
	rotView._14 = 0.0f;
	rotView._24 = 0.0f;
	rotView._34 = 0.0f;
	vso.pos = mul(u_Projection, mul(rotView, float4(Input.pos, 1.0f)));
	vso.pos.z = vso.pos.w;
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
	//float3 envColor = environmentMap.Sample(splr, dir).rgb;
	float3 envColor = environmentMap.SampleLevel(splr, dir,0).rgb;

	envColor = envColor / (envColor + float3(1.0, 1.0, 1.0));
	envColor = pow(envColor, 1.0 / 2.2);
	output.color = float4(envColor, 1.0f);
	//output.color = float4(1.0f,1.0f,1.0f,1.0f);
	return output;
}