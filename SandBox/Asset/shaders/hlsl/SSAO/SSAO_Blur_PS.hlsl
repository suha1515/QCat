
Texture2D ssaoInput;
SamplerState splr : register(s0);

struct PS_OUT
{
	float color : SV_TARGET0;
};
PS_OUT main(float2 tc : Texcoord) : SV_TARGET
{
	PS_OUT output;
	float2 texelSize;
	ssaoInput.GetDimensions(texelSize.x, texelSize.y);
	texelSize.x = 1.0f / texelSize.x;
	texelSize.y = 1.0f / texelSize.y;
	float result = 0.0f;
	[unroll]
	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			float2 offset = float2(float(x), float(y)) * texelSize;
			result += ssaoInput.Sample(splr, tc+ offset).r;
		}
	}
	output.color = result / (4.0f * 4.0f);
	return output;
}