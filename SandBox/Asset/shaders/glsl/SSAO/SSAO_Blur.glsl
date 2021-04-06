// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoords;

void main()
{
	TexCoords = a_TexCoord;
	gl_Position = vec4(a_Position,1.0);
}

#type fragment
#version 330 core
layout(location = 0) out float color;

in vec2 TexCoords;

uniform sampler2D ssaoInput;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput,0));
	float result = 0.0;
	for(int x=-2;x<2;++x)
	{
		for(int y=-2;y<2;++y)
		{
			vec2 offset = vec2(float(x),float(y))* texelSize;
			result +=texture(ssaoInput,TexCoords + offset).r;
		}
	}
	color = result / (4.0 * 4.0);
}