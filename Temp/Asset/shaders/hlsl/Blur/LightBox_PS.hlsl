cbuffer u_Color : register(b0)
{
	float4 u_color;
}
struct PS_OUT
{
	float4 color :SV_TARGET0;
	float4 BrightColor : SV_TARGET1;
};
PS_OUT main() 
{
	PS_OUT output;
	output.color = u_color;

	float brightness = dot(output.color.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	if (brightness > 0.99f)
		output.BrightColor = float4(output.color.rgb, 1.0f);
	else
		output.BrightColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//output.BrightColor = u_color;
	return output;
}