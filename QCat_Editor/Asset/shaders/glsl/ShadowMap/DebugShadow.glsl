#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

struct VertexOutput
{
	vec2 TexCoords;
};

layout(location = 0 ) out VertexOutput Output;

void main()
{
	Output.TexCoords = a_TexCoord;
	gl_Position = vec4(a_Position.x,a_Position.y,0.0,1.0);
}

#type fragment
#version  450 core
layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec2 TexCoords;
};

layout(location = 0 ) in VertexOutput Input;
layout(binding = 0 ) uniform sampler2D screenTexture;

void main()
{
	float depthValue = texture(screenTexture,Input.TexCoords).r;
	color = vec4(vec3(depthValue),1.0);

}