// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_BitTangent;


layout(std140,binding =0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
	vec3 viewPosition;
};
layout(std140,binding =1) uniform Transform
{
	mat4 u_Transform;
	mat4 u_InvTransform;
};

struct VertexOutput
{
	vec2 TexCoords;
	vec3 v_Normal;
	vec3 FragPos;
	mat3 TBN;
};
layout(location =0 )out VertexOutput Output;

void main()
{
	Output.TexCoords = a_TexCoord;
	mat3 normalMatrix = mat3(transpose(u_InvTransform));
	Output.v_Normal =normalMatrix * a_Normal;
	gl_Position = u_Projection*u_View*u_Transform * vec4(a_Position, 1.0);
	Output.FragPos = vec3(u_Transform * vec4(a_Position,1.0));

	vec3 T = normalize(normalMatrix *a_Tangent);
	vec3 B = normalize(normalMatrix *a_BitTangent);
	vec3 N = normalize(normalMatrix *a_Normal);
	// re-orthogonalize T With to N
	//T = normalize(T - dot(T,N) * N);
	//vec3 B = cross(N,T);
	Output.TBN = mat3(T,B,N);

}

#type fragment
#version 450 core
struct Material
{
	vec3 diffuse;
	vec3 specular;
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
layout(std140,binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
	vec3 u_viewPosition;
};
layout(std140,binding = 2) uniform Mat
{
	Material material;
};
layout(std140,binding = 3) uniform Light
{
	PointLight pointLight[1];
};
layout(std140,binding = 4) uniform Extra
{
	bool gamma;
	bool flip;
};
layout(location = 0) out vec4 color;

struct VertexOutput
{
    vec2 TexCoords;
	vec3 v_Normal;
	vec3 FragPos;
	mat3 TBN;
};
layout(location = 0 ) in VertexOutput Input;

layout (binding = 0) uniform sampler2D albedoMap;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D specularMap;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec4 texcolor = texture(albedoMap,Input.TexCoords);
	if(texcolor.a<0.1)
		discard;
	vec3 norm;
	vec3 viewDir;
	viewDir =  normalize(u_viewPosition - Input.FragPos);
	if(!material.normalMap)
	{
	   norm = normalize(Input.v_Normal);
	}
	else
	{
		norm = texture(normalMap,Input.TexCoords).rgb;
		norm.r = norm.r *2.0 - 1.0;
		norm.g = norm.g *2.0 - 1.0;
		norm.b = norm.b *2.0 - 1.0;
		norm = normalize(Input.TBN * norm);
	}	
	if(flip)
		norm = -norm;
		
	vec3 result;
	// dir light
	//result +=CalcDirLight(dirLight,norm,viewDir);
	 //point light
	result +=CalcPointLight(pointLight[0],norm,Input.FragPos,viewDir);

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
    vec3 ambient = light.ambient * vec3(texture(albedoMap, Input.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(normalMap, Input.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, Input.TexCoords));
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
    vec3 ambient = light.ambient * vec3(texture(albedoMap, Input.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(normalMap, Input.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, Input.TexCoords));
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