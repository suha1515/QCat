#type vertex

cbuffer Transform : register(b0)
{
	matrix u_Transform;
	matrix u_InvTransform;
}
struct VSIn
{
	float3 pos : a_Position;
	float3 normal : a_Normal;
	float2 tc  : a_TexCoord;
};
struct VSOut
{
	float2 tc		: TexCoords;
	float4 position : SV_POSITION;
};

VSOut VSMain( VSIn input)
{
	VSOut vso;
	vso.tc = input.tc;
	vso.position = mul(u_Transform, float4(input.pos, 1.0f));

	return vso;
}

#type geometry
cbuffer LightTransform : register (b1)
{
	matrix shadowMatrices[6];
}
struct GSIn
{
	float2 tc		: TexCoords;
	float4 position : SV_POSITION;
};
struct GSOutput
{
	float2 tc		: TexCoords;
	float4 FragPos  : FragPos;
	float4 pos		: SV_POSITION;
	uint RTIndex	: SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GSMain(
	triangle GSIn input[3],
	inout TriangleStream< GSOutput > output
)
{
	for (int face = 0; face < 6; ++face)
	{
		GSOutput element;
		element.RTIndex = face;
		for (uint i = 0; i < 3; i++)
		{
			element.FragPos = input[i].position;
			element.pos = mul(shadowMatrices[face], input[i].position);
			element.tc = input[i].tc;
			output.Append(element);
		}
		output.RestartStrip();
	}
}
#type pixel
struct PSIn
{
	float2 tc		: TexCoords;
	float4 FragPos  : FragPos;
	float4 pos		: SV_POSITION;
};

struct PS_OUT
{
	float depth : SV_Depth;
};

void PSMain(PSIn input)
{
}
