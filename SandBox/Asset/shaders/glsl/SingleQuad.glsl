// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoords;

void main()
{
	TexCoords = a_TexCoord;
	gl_Position = vec4(a_Position.x,a_Position.y,0.0,1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec3 FragColor = texture(screenTexture,TexCoords).rgb;
	color = vec4(FragColor,1.0f);
}