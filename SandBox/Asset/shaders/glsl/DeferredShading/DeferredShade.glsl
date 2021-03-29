// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;


out vec2 TexCoords;

void main()
{
	TexCoords = a_TexCoord;
	gl_Position =vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
struct PointLight
{
	vec3 position;

	float constant;
	float Linear;
	float quadratic;
    float radius;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
    
};
layout(location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D aAlbedoSpec;

const int NUM_LIGHT = 64;
uniform PointLight pointLight[NUM_LIGHT];
uniform vec3 viewPosition;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec4 albedoSpec);

void main()
{
	vec3 FragPos = texture(gPosition,TexCoords).rgb;
    vec3 Normal = texture(gNormal,TexCoords).rgb;
    vec4 DiffuseSpec = texture(aAlbedoSpec,TexCoords);

    vec3 result= vec3(0.0f,0.0f,0.0f);
    vec3 viewDir = normalize(viewPosition - FragPos);
    for(int i=0;i<NUM_LIGHT;++i)
    { 
        float distance = length(pointLight[i].position - FragPos);
        if(distance < pointLight[i].radius)
             result +=CalcPointLight(pointLight[i],Normal,FragPos,viewDir,DiffuseSpec);   
    }
    

    color = vec4(result,1.0f);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec4 albedoSpec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal,halfwayDir),0.0),16.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient   * albedoSpec.rgb;
    vec3 diffuse  = light.diffuse   * diff * albedoSpec.rgb;
    vec3 specular = light.specular  * spec  * albedoSpec.a;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
    //return vec3(spec,spec,spec);
}
