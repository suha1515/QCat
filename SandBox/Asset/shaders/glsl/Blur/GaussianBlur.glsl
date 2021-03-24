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

uniform sampler2D image;
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027,0.1945846,0.1216216,0.054054,0.016216);
void main()
{
	vec2 tex_offset = 1.0 / textureSize(image,0);				// get size of single texel
	vec3 result = texture(image,TexCoords).rgb * weight[0];		// current fragment's contribution

	if(horizontal)
	{
		for(int i=0;i<5;++i)
		{
			result +=texture(image,TexCoords + vec2(tex_offset.x * i,0.0f)).rgb * weight[i];
			result +=texture(image,TexCoords - vec2(tex_offset.x * i,0.0f)).rgb * weight[i];
		}
	}
	else
	{
		for(int i=0;i<5;++i)
		{
			result +=texture(image,TexCoords + vec2(0.0f,tex_offset.y * i)).rgb * weight[i];
			result +=texture(image,TexCoords - vec2(0.0f,tex_offset.y * i)).rgb * weight[i];
		}
	}

	color = vec4(result,1.0f);
}