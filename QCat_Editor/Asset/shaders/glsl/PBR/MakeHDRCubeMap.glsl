// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140,binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
};


struct VertexOutput
{
   vec3 localPos;
};
layout(location = 0 ) out VertexOutput Output;
void main()
{
	Output.localPos = a_Position;
	mat4 rotView = mat4(mat3(u_View));//remove translation
	vec4 clipPos = u_Projection * rotView * vec4(a_Position , 1.0f);
	gl_Position = clipPos.xyww;
	//gl_Position = clipPos;
}

#type fragment
#version 450 core
layout(location = 0) out vec4 color;

struct VertexOutput
{
   vec3 localPos;
};
layout(location = 0 ) in VertexOutput Input;

layout (binding = 0) uniform samplerCube environmentMap;


void main()
{
	vec3 dir = Input.localPos;
	
	
	vec3 envColor = texture(environmentMap,dir).rgb;
	//vec3 envColor = textureLod(environmentMap, dir, 1.2).rgb; 

	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor,vec3(1.0/2.2));

	color = vec4(envColor,1.0);
}
