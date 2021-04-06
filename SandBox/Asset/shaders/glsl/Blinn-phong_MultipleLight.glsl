// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_BitTangent;


uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec3 viewPosition;

out vec2 TexCoords;
out vec3 v_Normal;
out vec3 FragPos;
out mat3 TBN;

void main()
{
	TexCoords = a_TexCoord;
	mat3 normalMatrix = mat3(transpose(inverse(u_Transform)));
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	gl_Position = u_ViewProjection *u_Transform * vec4(a_Position, 1.0);
	FragPos = vec3(u_Transform * vec4(a_Position,1.0));

	vec3 T = normalize(normalMatrix *a_Tangent);
	vec3 B = normalize(normalMatrix *a_BitTangent);
	vec3 N = normalize(normalMatrix *a_Normal);
	// re-orthogonalize T With to N
	//T = normalize(T - dot(T,N) * N);
	//vec3 B = cross(N,T);
	TBN = mat3(T,B,N);

}

#type fragment
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
layout(location = 1) out vec4 BrightColor;

in vec3 v_Normal;
in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform Material material;
uniform PointLight pointLight[4];
uniform DirLight dirLight;
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
	for(int i=0;i<4;++i)
		result +=CalcPointLight(pointLight[i],norm,FragPos,viewDir);

	if(gamma)
		result = pow(result,vec3(1.0/2.2));
	color = vec4(result,texcolor.a);

	float brightness = dot(color.rgb,vec3(0.2126,0.7152,0.0722));
	if(brightness>1.0)
		BrightColor = vec4(color.rgb,1.0f);
	else
		BrightColor = vec4(0.0f,0.0f,0.0f,0.0f);
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