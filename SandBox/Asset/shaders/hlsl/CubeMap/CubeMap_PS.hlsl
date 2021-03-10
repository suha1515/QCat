
TextureCube skybox : register(t0);
SamplerState splr : register(s0);

struct PS_OUT
{
	float4 color :SV_TARGET0;
};

struct PSIn
{
	float3 tc			   : Texcoord;
	float4 pos			   :SV_Position;
};
PS_OUT main(PSIn input)
{
	PS_OUT output;
	float red = skybox.Sample(splr, input.tc).r;
	output.color = float4(red, red, red, 1.0f);
	return output;
}