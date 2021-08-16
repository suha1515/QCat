#type vertex
#version 450 core
struct VertexOutput
{
	vec2 uv;
};
layout(location = 0 ) out VertexOutput Output;

void FullScreenTriangle(in uint index, out vec4 pos, out vec2 uv)
{
	pos.x = (index/2) * 4.0 - 1.0;
	pos.y= (index%2) * 4.0 - 1.0;
	pos.z = 0.0;
	pos.w = 1.0;

	uv.x = (index/2) *2;
	uv.y = (index%2) *2;
}
void main()
{
	vec4 pos;
	vec2 uv;

	FullScreenTriangle(gl_VertexIndex,pos,uv);

	Output.uv = uv;
	gl_Position = pos;
}
#type fragment
#version 450 core
struct VertexInput
{
	vec2 uv;
};

layout(std140,binding = 0 ) uniform OutlineInfo
{
	vec4 outlineColor;
	float outlineThickness;
	float outlineThreshHold;
};
layout(location = 0 ) in VertexInput Input;
layout(location = 0) out vec4 color;
layout (binding =0 ) uniform sampler2D image;
float xFilter[9] = { -1,0,1,-2,0,2,-1,0,1};
float yFilter[9] = {-1,-2,-1,0,0,0,1,2,1};
void main()
{
	vec2 size = textureSize(image,0).xy;

	vec2 tx = vec2(outlineThickness/size.x , 0.0);
	vec2 ty = vec2(0.0 ,outlineThickness/size.y);

	float grid[9];
	vec3 greyScale = vec3(0.299, 0.587, 0.114);
	grid[0] = length(dot(texture(image,Input.uv - tx - ty).xyz,greyScale));
	grid[1] = length(dot(texture(image,Input.uv - ty).xyz,greyScale));
	grid[2] = length(dot(texture(image,Input.uv + tx - ty).xyz,greyScale));
	grid[3] = length(dot(texture(image,Input.uv - tx).xyz,greyScale));
	grid[4] = length(dot(texture(image,Input.uv).xyz,greyScale));
	grid[5] = length(dot(texture(image,Input.uv + tx).xyz,greyScale));
	grid[6] = length(dot(texture(image,Input.uv - tx + ty).xyz,greyScale));
	grid[7] = length(dot(texture(image,Input.uv + ty).xyz,greyScale));
	grid[8] = length(dot(texture(image,Input.uv + tx + ty).xyz,greyScale));

	float sx,sy;
	for(int i=0;i<9;++i)
	{
		sx += grid[i] * xFilter[i];
		sy += grid[i] * yFilter[i];
	}
	
	float dist = sqrt(sx*sx + sy*sy);

	float edge = dist > outlineThreshHold ? 1:0;

	vec4 result = outlineColor;
	result.a = result.a * edge;
	color = result;
}