// Flat Color Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140,binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
};

layout(std140,binding = 1) uniform Transform
{
	mat4 u_Transform;
};
void main()
{
	gl_Position = u_Projection * u_View * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(std140,binding = 1) uniform col
{
	vec4 u_color;
};

void main()
{
	color = u_color;
}