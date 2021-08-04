// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoords;

layout(std140,binding =0) uniform Transform
{
	mat4 u_Transform;
	mat4 u_InvTransform;
};
layout(std140,binding = 1) uniform ShadowTransform
{
	mat4 lightTransform;
};
struct VertexOutput
{
	vec2 TexCoords;
};
layout(location =0 )out VertexOutput Output;

void main()
{
	Output.TexCoords = a_TexCoords;
	gl_Position = lightTransform * u_Transform * vec4(a_Position,1.0);
}
#type fragment
#version 450 core
struct VertexOutput
{
	vec2 Texcoords;
};
layout(location = 0) in VertexOutput Input;

void main()
{
}

