//Cascade Shadow Map applied
#type vertex
struct VSIn
{
	float3 pos : a_Position;
	float3 normal : a_Normal;
	float2 tc  : a_TexCoord;
};

cbuffer Transform : register(b0)
{
	matrix u_Transform;
	matrix u_InvTransform;
}
struct VSOut
{
	float2 tc		: Texcoords;
	float4 position : SV_POSITION;
};

VSOut VSMain(VSIn input)
{
	VSOut output;
	output.tc = input.tc;
	output.position = mul(u_Transform, float4(input.pos, 1.0f));
	return output;
}
#type geometry
cbuffer LightTransform : register(b1)
{
	matrix lightTransform[5];
}
struct GSIn
{
	float2 tc : Texcoords;
	float4 position : SV_POSITION;
};
struct GSOutput
{
	float2 tc : Texcoords;
	float4 pos : SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(15)]
void GSMain(triangle GSIn input[3], inout TriangleStream <GSOutput> output)
{
	for (int face = 0; face < 5; ++face)
	{
		GSOutput element;
		element.RTIndex = face;
		for (int i = 0; i <3; ++i)
		{
			element.pos = mul(lightTransform[face], input[i].position);
			element.tc = input[i].tc;
			output.Append(element);
		}
		output.RestartStrip();
	}
}
#type pixel
struct PSIn
{
	float2 tc		: Texcoords;
	float4 position : SV_POSITION;
};
void PSMain(PSIn input)
{
}