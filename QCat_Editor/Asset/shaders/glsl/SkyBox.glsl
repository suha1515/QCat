#type vertex
#version 450 core
layout(location = 0) in vec3 a_Position;

layout(std140,binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
};

layout(location = 0 ) out vec3 TexCoords;

void main()
{
	TexCoords = a_Position;
	vec4 pos = u_Projection * u_View * vec4(a_Position,1.0);
	gl_Position = pos.xyww; 

}

#type fragment
#version 450 core
layout(location = 0) out vec4 color;
layout(location = 0 ) in vec3 TexCoords;
layout(binding = 0 ) uniform samplerCube skybox;

void main()
{
	color = texture(skybox,TexCoords);	
}

