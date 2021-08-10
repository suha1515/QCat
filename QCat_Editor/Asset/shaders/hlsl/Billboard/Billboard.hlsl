#type vertex
cbuffer Camera : register(b0)
{
	matrix u_Projection;
	matrix u_View;
	float3 u_viewPosition;
}
cbuffer u_Transform : register(b1)
{
	float4 corners[4];
}
struct VSOut
{
	float4 pos : SV_POSITION;
	float2 q   : TEXCOORD1;
	float2 b1  : TEXCOORD2;
	float2 b2  : TEXCOORD3;
	float2 b3  : TEXCOORD4;
};
VSOut VSMain(uint vertexID : SV_VertexID)
{
	VSOut vso;

	float4 pos = corners[vertexID];
	vso.q =  (pos - corners[0]).xy;
	vso.b1 = (corners[2] - corners[0]).xy;
	vso.b2 = (corners[1] - corners[0]).xy;
	vso.b3 = (corners[0] - corners[1]-corners[2] + corners[3]).xy;

	vso.pos = mul(u_Projection, mul(u_View, pos));
	return vso;
}
#type pixel
float Wedge2D(float2 v, float2 w)
{
	return v.x * w.y - v.y * w.x;
}
struct PSIn
{
	float4 pos : SV_POSITION;
	float2 q   : TEXCOORD1;
	float2 b1  : TEXCOORD2;
	float2 b2  : TEXCOORD3;
	float2 b3  : TEXCOORD4;
};
Texture2D image : register(t0);
SamplerState imagesplr : register(s0);
float4 PSMain(PSIn Input) : SV_TARGET
{
	float4 color;
	float A = Wedge2D(Input.b2, Input.b3);
	float B = Wedge2D(Input.b3, Input.q) - Wedge2D(Input.b1, Input.b2);
	float C = Wedge2D(Input.b1, Input.q);

	//Solve For V
	float2 uv;
	if (abs(A) < 0.001)
	{
		uv.y = -C / B;
	}
	else
	{
		// Quadratic form , Take Positive root for CCW winding with V-Up
		float discrim = B * B - 4 * A * C;
		uv.y = 0.5 * (-B + sqrt(discrim)) / A;
	}
	float2 denom = Input.b1 + uv.y * Input.b3;
	if (abs(denom.x) > abs(denom.y))
		uv.x = (Input.q.x - Input.b2.x * uv.y) / denom;
	else
		uv.y = (Input.q.y - Input.b2.y * uv.y) / denom;

	// Opengl to dx 
	uv = float2 (uv.x, 1 - uv.y);
	color = image.Sample(imagesplr, uv);
	if (color.a < 0.1)
		discard;
	return color;
}