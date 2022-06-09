// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoords;

void main()
{
	TexCoords = a_TexCoord;
	gl_Position = vec4(a_Position,1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct PointLight
{
	vec3 position;

	float constant;
	float Linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform PointLight pointLight[1];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec4 albedoSpec,float ambientOcclusion);
void main()
{
	// retrieve information
	vec3 FragPos = texture(gPosition,TexCoords).rgb;
	vec3 Normal  = texture(gNormal,TexCoords).rgb;
	vec4 albedoSpec = texture(gAlbedo,TexCoords);
	float AmbientOcclusion = texture(ssao,TexCoords).r;

	//Caclculate Light
	vec3 viewDir = normalize(-FragPos);

	vec3 result = vec3(0.0f);
	result = CalcPointLight(pointLight[0],Normal,FragPos,viewDir,albedoSpec,AmbientOcclusion);

	color = vec4(result,1.0f);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec4 albedoSpec,float ambientOcclusion)
{
    vec3 lightDir = normalize(light.position - fragPos);
	float spec;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal,halfwayDir),0.0),64.0f);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * albedoSpec.rgb * (ambientOcclusion);
    vec3 diffuse = light.diffuse * diff * albedoSpec.rgb;
    vec3 specular = light.specular * spec * albedoSpec.a;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
