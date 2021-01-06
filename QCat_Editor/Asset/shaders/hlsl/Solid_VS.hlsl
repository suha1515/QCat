cbuffer modelViewProj : register(b0)
{
	matrix viewProj;
}

struct VSOut
{
	float4 color : COLOR;
	float4 pos : SV_POSITION;
};

VSOut main( float3 pos : Position, float4 color :Color)
{
	VSOut vso;

	vso.pos = mul( viewProj, float4(pos, 1.0f));
	vso.color = color;
	return vso;
}