// Flat Color Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TextureCoords;

layout(location = 0) out vec3 out_Color;
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
	out_Color = a_Color;
	gl_Position = u_Projection * u_View * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core
layout(location = 1) in vec3 out_Color;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(out_Color,1.0f);
}