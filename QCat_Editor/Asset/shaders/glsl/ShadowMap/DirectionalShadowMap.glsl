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

struct VertexOutput
{
	vec2 TexCoords;
};
layout(location =0 )out VertexOutput Output;

void main()
{
	Output.TexCoords = a_TexCoords;
	gl_Position = u_Transform * vec4(a_Position,1.0);
}
#type geometry
#version 450 core

layout(triangles) in;
layout(triangle_strip,max_vertices=15) out;
const int NUM_CASCADES = 3;
layout(std140,binding = 1) uniform ShadowTransform
{
	mat4 lightTransform[5];
};
struct VertexOutput
{
	vec2 TexCoords;
};
layout(location = 0 ) in VertexOutput Input[];

struct GeometryOutput
{
	vec4 FragPos;
	vec2 Texcoords;
};
layout(location = 1) out GeometryOutput Output;

void main()
{
	for(int face = 0;face<5;++face)
	{
		gl_Layer = face;
		for(int i=0;i<3;++i)
		{
			Output.FragPos = gl_in[i].gl_Position;
			gl_Position = lightTransform[face] * Output.FragPos;
			Output.Texcoords = Input[i].TexCoords;
			EmitVertex();
		}
		EndPrimitive();
	}
}
#type fragment
#version 450 core
struct GeometryOutput
{
	vec4 FragPos;
	vec2 Texcoords;
};
layout(location = 1) in GeometryOutput Input;

void main()
{
}

