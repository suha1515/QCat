#version 330 core
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D emission;
	float shininess;

	bool normalMap;
};
struct DirLight
{
	vec3 direction;
	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
};
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

//struct SpotLight
//{
//	vec3 position;
//	vec3 direction;
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//
//	float constant;
//	float Linear;
//	float quadratic;
//
//	float cutOff;
//	float outerCutOff;
//};

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform Material material;
uniform PointLight pointLight;
uniform vec3 viewPosition;
uniform bool gamma;
uniform bool flip;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec4 texcolor = texture(material.diffuse,TexCoords);
	if(texcolor.a<0.1)
		discard;
	vec3 norm;
	vec3 viewDir;
	viewDir =  normalize(viewPosition - FragPos);
	if(!material.normalMap)
	{
	   norm = normalize(v_Normal);
	}
	else
	{
		norm = texture(material.normal,TexCoords).rgb;
		norm.r = norm.r *2.0 - 1.0;
		norm.g = norm.g *2.0 - 1.0;
		norm.b = norm.b *2.0 - 1.0;
		norm = normalize(TBN * norm);
	}	
	if(flip)
		norm = -norm;
		
	vec3 result;
	// dir light
	//result +=CalcDirLight(dirLight,norm,viewDir);
	 //point light
	result +=CalcPointLight(pointLight,norm,FragPos,viewDir);

	if(gamma)
		result = pow(result,vec3(1.0/2.2));
	color = vec4(result,texcolor.a);
	//color = vec4(norm,1.0f);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

	float spec;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal,halfwayDir),0.0),material.shininess*4);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + (diffuse + specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
	float spec;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal,halfwayDir),0.0),material.shininess*4);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(light.position - fragPos);
//    // diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    // attenuation
//    float distance = length(light.position - fragPos);
//    float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));    
//    // spotlight intensity
//    float theta = dot(lightDir, normalize(-light.direction)); 
//    float epsilon = light.cutOff - light.outerCutOff;
//    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//    // combine results
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
//    ambient *= attenuation * intensity;
//    diffuse *= attenuation * intensity;
//    specular *= attenuation * intensity;
//    return (ambient + diffuse + specular);
//}