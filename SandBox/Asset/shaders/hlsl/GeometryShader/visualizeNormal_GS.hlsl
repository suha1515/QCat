struct GSIn
{
	float4 position : Position;
	float3 normal : Normal;
};
struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(6)]
void main(
	triangle GSIn input[3] : SV_POSITION,
	inout LineStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i].position;
		output.Append(element);

		element.pos = input[i].position + (float4(input[i].normal,0.0f) * 0.4f);
		output.Append(element);
		output.RestartStrip();
	}
}