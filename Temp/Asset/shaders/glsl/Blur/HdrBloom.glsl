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

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer,TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur,TexCoords).rgb;
	hdrColor +=bloomColor;
	// exposure tone mapping
	//vec3 mapped = hdrColor /( hdrColor + vec3(1.0f));
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	// gamma correction
	mapped = pow(mapped,vec3(1.0/gamma));

	color = vec4(mapped,1.0);
}