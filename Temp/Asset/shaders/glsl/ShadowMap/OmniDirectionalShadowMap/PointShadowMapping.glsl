// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec2 a_TexCoords;

out VS_OUT
{
	vec2 tc;
}vs_out;

uniform mat4 u_Transform;

void main()
{
	vs_out.tc = a_TexCoords;
	gl_Position = u_Transform * vec4(a_Position,1.0);
}
#type geometry
#version 330 core

layout(triangles)in;
layout(triangle_strip,max_vertices=18) out;

uniform mat4 shadowMatrices[6];

in VS_OUT
{
	vec2 tc;
}gs_in[];

out vec4 FragPos;
out vec2 texcoords;

void main()
{
	for(int face=0;face<6;++face)
	{
		gl_Layer = face;
		for(int i=0;i<3;++i)
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos ;
			texcoords = gs_in[i].tc;
			EmitVertex();
		}
		EndPrimitive();
	}
}

#type fragment
#version 330 core

in vec4 FragPos;
in vec2 texcoords;

uniform vec3 lightPos;
uniform float far_plane;

uniform sampler2D diffuseTex;
void main()
{

	vec4 texcolor = texture(diffuseTex,texcoords);
	if(texcolor.a<0.1)
		discard;
	//float lightDistance = length(FragPos.xyz - lightPos);
	//lightDistance = lightDistance / far_plane;
	vec3 lightToFrag = FragPos.xyz - lightPos;
}
