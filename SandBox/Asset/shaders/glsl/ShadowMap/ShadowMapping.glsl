// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoords;

out vec2 tc;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	tc = a_TexCoords;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position,1.0);
}

#type fragment
#version 330 core

in vec2 tc;

uniform sampler2D diffuseTex;

void main()
{
	vec4 texcolor = texture(diffuseTex,tc);
	if(texcolor.a<0.1)
		discard;
}
