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

layout(std140,binding = 0 ) uniform Color
{
	vec4 screenColor;
};

layout(location = 0 ) in VertexInput Input;
layout(location = 0) out vec4 color;
layout (binding =0 ) uniform sampler2D image;

void main()
{
	color = texture(image,Input.uv) * screenColor;
}