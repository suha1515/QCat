cbuffer value : register(b0)
{
	float length;
}
struct GSIn
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float4 fragPosLightSpace : FragPosLightSpace;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
};

struct GSOutput
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float4 fragPosLightSpace : FragPosLightSpace;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
};

float3 GetNormal(GSIn input[3])
{
	float3 a = (float3)input[0].pos - (float3)input[1].pos;
	float3 b = (float3)input[2].pos - (float3)input[1].pos;
	return normalize(cross(b, a));
}
float4 explode(float4 position, float3 normal)
{
	float magnitude = 2.0;
	float3 direction = normal * length * magnitude;
	return position + float4(direction, 0.0);
}
[maxvertexcount(3)]
void main(
	triangle GSIn input[3] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		float3 normal = GetNormal(input);
		element = input[i];

		element.pos = explode(input[i].pos, normal);
		output.Append(element);	
	}
	output.RestartStrip();
}