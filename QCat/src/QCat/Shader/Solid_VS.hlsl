struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

VSOut main( float3 pos : POSITION )
{
	VSOut vso;
	vso.pos = float4(pos.x,pos.y,0.0f,1.0f);
	vso.color =float3(1.0f,1.0f,1.0f);
	return vso;
}