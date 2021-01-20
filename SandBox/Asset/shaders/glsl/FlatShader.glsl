// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;


uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 FragPos;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	gl_Position = u_ViewProjection *u_Transform * vec4(a_Position, 1.0);
	FragPos = vec3(u_Transform * vec4(a_Position,1.0));
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightAmbient;
uniform vec3 viewPos;
uniform float specularstrength;
uniform int shininess;
uniform sampler2D u_Texture;


void main()
{
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);

	float spec = pow(max(dot(viewDir,reflectDir),0.0),shininess);
	vec3 specular = specularstrength * spec * lightColor;

	float diff = 1.0;
	diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff * lightColor;
	vec4 texColor = texture(u_Texture, v_TexCoord ) ; 

	color = texColor * vec4(diffuse + lightAmbient +specular,1.0);
}