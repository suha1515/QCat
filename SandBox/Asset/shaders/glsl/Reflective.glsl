#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat4 u_invTransform;

out vec3 Normal;
out vec3 Position;

void main()
{
	Normal = mat3(transpose(u_invTransform)) * a_Normal;
	Position = vec3(u_Transform * vec4(a_Position,1.0));
	gl_Position = u_ViewProjection * vec4(Position,1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 Normal;
in vec3 Position;

uniform vec3 viewPosition;
uniform samplerCube skybox;

void main()
{
	vec3 I = normalize(Position - viewPosition);
	// Reflection
	//vec3 R = reflect(I,normalize(Normal));
	// Refract
	float ratio = 1.00/1.52;
	vec3 R = refract(I,normalize(Normal),ratio);
	color = vec4(texture(skybox,R).rgb,1.0);
	//color = vec4(viewPosition,1.0);
}

