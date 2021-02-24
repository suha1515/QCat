#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 TexCoords;

void main()
{
	TexCoords = a_Position;
	vec4 pos = u_ViewProjection *u_Transform* vec4(a_Position,1.0);
	gl_Position = pos; 

}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	vec3 tex = TexCoords;
	//tex.y = tex.y * -1.0;
	//tex.z = tex.z*-1.0;
	//tex.x = tex.x*-1.0;
	color = texture(skybox,-tex);	
}

