#type compute
#version 450 core

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

//layout (binding=0, rgba8)  uniform  readonly image2D inputImg;
layout (binding = 0) uniform sampler2D inputImg;
layout (binding=1, rgba8)  uniform  writeonly image2D outputImg;

shared vec4 gCache[266];
static float weights[11] = {0.05f,0.05f,0.1f,0.1f,0.2f,0.1f,0.1f,0.1f,0.05f,0.05f};
void main()
{
	ivec2 imgSize = imageSize(inputImg);
	uvec2 globalindex = gl_GlobalInvocationID.xy;
	uvec2 groupindex = gl_LocalInvocationID.xy;
	vec2 coord = globalindex / imgSize;
	if(globalindex.x < imgSize.x && globalindex.y <imgeSize.y)
	{
		if(groupindex.x < 5)
		{
			int x = max(globalindex.x - 5 , 0);
			//gCache[groupindex.x] = imgaeLoad(inputImg,ivec2(x,globalindex.y));
			gCache[groupindex.x] = textureLod(inputImg,coord,0);
		}
		if(groupindex.x >= 251)
		{
			int x= min(globalindex.x + 5,imgSize.x -1);
			//gCache[groupindex.x + 10] = imageLoad(inputImg,ivec2(x,globalindex.y));
			gCache[groupindex.x + 10] = textureLod(inputImg,coord,0);
		}
		//gCache[groupindex.x + 5] = imageLoad(inputImg,vec2(min(globalindex,imgSize.xy-1)));
		gCache[groupindex.x + 5] = textureLod(inputImg,coord,0);
	}
	barrier();

	vec4 blurColor = vec4(0.0,0.0,0.0,0.0);

	for(int i=-5;i<=5;++i)
	{
		int k = groupindex.x + 5 + i;
		blurColor = gCacge[k] * weights[i+5];
	}

	imgStore(outputImg,globalindex,blurColor);

}