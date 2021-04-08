#type vertex
cbuffer trnasform  : register(b0)
{
	matrix u_Projection;
	matrix u_View;
	matrix u_Transform;
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
	vso.pos = mul(u_Projection, mul(u_View,mul(u_Transform, float4(Input.pos, 1.0f))));
	
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

Texture2D equirectangularMap;
SamplerState splr : register(s0);

static const float2 invAtan = float2(0.1591f, -0.3183f);

float2 SamplerSphericalMap(float3 v)
{
	float2 uv = float2(atan2(v.x, v.z), asin(v.y));
	uv *= invAtan;
	uv += 0.5f;
	return uv;
}

PS_OUT PSMain(PSIn input)
{
	PS_OUT output;
	
	float2 uv = SamplerSphericalMap(normalize(input.localPos));
	float3 color = equirectangularMap.Sample(splr, uv).rgb;
	output.color = float4(color, 1.0f);
	return output;
}