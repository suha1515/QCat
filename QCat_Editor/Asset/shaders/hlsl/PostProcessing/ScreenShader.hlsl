#type vertex
struct VSOut
{
	float2 uv  : TEXTURE;
	float4 pos : SV_POSITION;
};

void FullScreenTriangle(in uint index, out float4 pos, out float2 uv)
{
	pos.x = float(index / 2) * 4.0 - 1.0;
	pos.y = float(index % 2) * 4.0 - 1.0;
	pos.z = 0.0;
	pos.w = 1.0;

	uv.x = float(index / 2) * 2;
	uv.y = 1- float(index % 2) * 2;
}
VSOut VSMain(uint vertexID : SV_VertexID)
{
	VSOut vso;

	float4 pos;
	float2 uv;
	FullScreenTriangle(vertexID, pos, uv);

	vso.pos = pos;
	vso.uv = uv;
	return vso;
}
#type pixel
cbuffer Color : register(b0)
{
	float4 color;
}
struct PSIn
{
	float2 uv  : TEXTURE;
	float4 pos : SV_POSITION;
};
Texture2D image : register(t0);
SamplerState imagesplr : register(s0);
struct PS_Out
{
	float4 color : SV_TARGET0;
};
PS_Out PSMain(PSIn Input)
{
	PS_Out output;
	output.color = image.Sample(imagesplr, Input.uv) * color;
	return output;
}