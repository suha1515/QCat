struct VSOut
{
	float2 tc : Texcoord;
	float4 position : SV_POSITION;
};

VSOut main( float2 pos : a_Position,float2 tc : a_TexCoords)
{
	VSOut vso;
	vso.position = float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.tc = tc;
	return vso;
}