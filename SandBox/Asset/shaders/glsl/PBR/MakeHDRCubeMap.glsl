// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 localPos;


void main()
{
	localPos = a_Position;
	mat4 rotView = mat4(mat3(u_View));//remove translation
	vec4 clipPos = u_Projection * u_View * vec4(localPos , 1.0f);
	//gl_Position = clipPos.xyww;
	gl_Position  = clipPos;
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 localPos;

uniform samplerCube environmentMap;

void main()
{
	vec3 dir = localPos;
	//dir.y = dir.y * -1.0;
	//dir.z = dir.z*-1.0;
	//dir.x = dir.x*-1.0;
	vec3 envColor = texture(environmentMap,dir).rgb;

	//envColor = envColor / (envColor + vec3(1.0));
	//envColor = pow(envColor,vec3(1.0/2.2));

	color = vec4(envColor,1.0);
}
