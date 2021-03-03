
Texture2D screenTex;
SamplerState splr :register(s0);
const float offsetX =1.0f/1600.0f;
const float offsetY = 1.0f / 900.0f;
float4 main(float2 tc : Texcoord) : SV_TARGET
{
	float4 color;
	float4 texColor = screenTex.Sample(splr, tc);
	
	//Inversion
	//color = float4(float3(1.0f-screenTex.Sample(splr, tc).rgb), 1.0f);
	// GrayScale
	//float average = ((0.2126 *texColor.r) + (0.7152 * texColor.g)+ (0.0722 * texColor.b));
	//color = float4(average, average, average, 1.0f);
	//Kernels
	float2 offsets[9] = 
	{
		float2(-offsetX, offsetY),	//top - left
		float2(0.0f, offsetY),	 //top - center
		float2(offsetX, offsetY),  //top - right
		float2(-offsetX, 0.0f),	 //center-left
		float2(0.0f, 0.0f),		 //center-center
		float2(offsetX, 0.0f),	 //center-right
		float2(-offsetX, -offsetY), //bottom - left
		float2(0.0f, -offsetY),	 //bottom - center
		float2(offsetX, -offsetY)  //bottom-right
	};

	/*float kernel[9] = {
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	};*/
	float kernel[9]=
	{
		1.0f / 16.0f , 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f , 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f , 2.0f / 16.0f, 1.0f / 16.0f
	};
	float3 sampleTex[9];
	/*sampleTex[0] = screenTex.Sample(splr, tc + float2(-offset, offset)).rgb;
	sampleTex[1] = screenTex.Sample(splr, tc + float2( 0.0f, offset)).rgb;
	sampleTex[2] = screenTex.Sample(splr, tc + float2(offset, offset)).rgb;
	sampleTex[3] = screenTex.Sample(splr, tc + float2(-offset, 0.0f)).rgb;
	sampleTex[4] = screenTex.Sample(splr, tc + float2(0.0f, 0.0f)).rgb;
	sampleTex[5] = screenTex.Sample(splr, tc + float2( offset, 0.0f)).rgb;
	sampleTex[6] = screenTex.Sample(splr, tc + float2(-offset, -offset)).rgb;
	sampleTex[7] = screenTex.Sample(splr, tc + float2( 0.0f, -offset)).rgb;
	sampleTex[8] = screenTex.Sample(splr, tc + float2(offset, -offset)).rgb;*/
	//float3 col = float3(0.0f, 0.0f, 0.0f);
	//[unroll]
	//for (int i = 0; i < 9; ++i)
	//{
	//	col += screenTex.Sample(splr, tc + offsets[i]).rgb * kernel[i];
	//	//sampleTex[i] = float3(1.0f, 1.0f, 1.0f);
	//}
	//color = float4(col, 1.0f);
	color = screenTex.Sample(splr, tc);
	return color;
}