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

TextureCube envMap;
SamplerState splr : register(s0);

static const float PI = 3.14159265359;

PS_OUT PSMain(PSIn input)
{
	PS_OUT output;
	
	float3 N = normalize(input.localPos);
	float3 irradiance = float3(0.0f, 0.0f, 0.0f);

	// tangent space calculationfrom origin point
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
	{
		for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
		{
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += envMap.Sample(splr, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	output.color = float4(irradiance, 1.0f);
	return output;
}