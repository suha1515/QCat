cbuffer LightTransform : register (b0)
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
void main(
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
