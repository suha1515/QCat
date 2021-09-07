#type compute
#version 450 core

layout (local_size_x = 1, local_size_y = 256, local_size_z = 1) in;

layout (binding = 0) uniform sampler2D inputImg;
//layout (binding=0, rgba8)  uniform  readonly image2D inputImg;
layout (binding=1, rgba8)  uniform  writeonly image2D outputImg;

shared vec4 gCache[266];
static float weights[11] = {0.05f,0.05f,0.1f,0.1f,0.2f,0.1f,0.1f,0.1f,0.05f,0.05f};
void main()
{
	vec2 imgSize = imageSize(inputImg);
	uvec2 globalindex = gl_GlobalInvocationID.xy;
	uvec2 groupindex = gl_LocalInvocationID.xy;
	vec2 coord = globalindex / imgSize;
	if(globalindex.x < imgSize.x && globalindex.y <imgeSize.y)
	{
		if(groupindex.y < 5)
		{
			int y = max(globalindex.y - 5 , 0);
			gCache[groupindex.y] = textureLod(inputImg,coord,0);
			//imgaeLoad(inputImg,ivec2(globalindex.x,y));
		}
		if(groupindex.y >= 251)
		{
			int y= min(globalindex.y + 5,imgSize.y -1);
			gCache[groupindex.y + 10] = textureLod(inputImg,coord,0);
			//gCache[groupindex.y + 10] = imageLoad(inputImg,ivec2(globalindex.x,y));
		}
		
		//gCache[groupindex.y + 5] = imageLoad(inputImg,vec2(min(globalindex,imgSize.xy-1)));
		gCache[groupindex.y + 5] =textureLod(inputImg,coord,0);
	}
	barrier();

	vec4 blurColor = vec4(0.0,0.0,0.0,0.0);

	for(int i=-5;i<=5;++i)
	{
		int k = groupindex.y + 5 + i;
		blurColor = gCacge[k] * weights[i+5];
	}

	imgStore(outputImg,globalindex,blurColor);

}