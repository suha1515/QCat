#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;

out vec3 TexCoords;

void main()
{
	TexCoords = a_Position;
	vec4 pos = u_ViewProjection * vec4(a_Position,1.0);
	gl_Position = pos.xyww; 

}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	color = texture(skybox,TexCoords);	
}

