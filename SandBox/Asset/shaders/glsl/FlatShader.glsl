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
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};
struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float Linear;
	float quadratic;
};

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 FragPos;

uniform Material material;
uniform Light light;
uniform vec3 viewPosition;

void main()
{
	// ambient
	vec3 ambient = light.ambient  * vec3(texture(material.diffuse,v_TexCoord));

	// diffuse
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff =  max(dot(norm,lightDir),0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,v_TexCoord));

	// specular
	vec3 viewDir = normalize(viewPosition - FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular,v_TexCoord));;

	// emission
	//vec3 emission = texture(material.emission,v_TexCoord).rgb;

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	color = vec4(diffuse + ambient + specular,1.0);
}