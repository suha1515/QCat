struct PSIn
{
	float2 tc		: TexCooords;
	float4 position : SV_POSITION;
};

Texture2D diffuseTex;
SamplerState splr :register(s0);

void main(PSIn input) 
{
	float4 texColor = diffuseTex.Sample(splr,input.tc);
	if (texColor.a < 0.1f)
		discard;

}