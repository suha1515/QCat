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

in vec3 v_Normal;
in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform Material material;
uniform PointLight pointLight;
uniform vec3 viewPosition;
uniform bool gamma;

uniform float height_scale;
uniform float far_plane;
uniform float near_plane;

uniform samplerCubeShadow depthMap;
uniform sampler2D heightMap;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   
float CalculationShadowDepth(vec3 shadowPos)
{
	// -1 t0 1 for z
	float c1 = (far_plane + near_plane) / (far_plane - near_plane);
	float c0 = -(2 * far_plane * near_plane) / (far_plane - near_plane);
	vec3 m = abs(shadowPos).xyz;
	float major = max(m.x, max(m.y, m.z));
	return (c1 * major + c0) / major;
}
float ParallaxShadow(vec3 lightdir,vec2 texCoords)
{
	vec3 tanlightDir = normalize(TBN * lightdir);
	// Steep ParrallaxMapping
	float numLayers =10;
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), lightdir), 0.0)); 

	float currentLayerDepth = 0.0f;
	float shadowFactor = 0.0f;
	
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(heightMap,currentTexCoords).r;

	currentLayerDepth = currentDepthMapValue;
	float layerDepth = (1.0f-currentDepthMapValue)/numLayers;
	layerDepth = 1.0f/numLayers;
	vec2 P = tanlightDir.xy * height_scale;
	vec2 deltaTexCoords = P / numLayers;

	while(currentLayerDepth <= currentDepthMapValue && currentLayerDepth>=0.0f)
	{
		currentTexCoords += deltaTexCoords;
		currentDepthMapValue = texture(heightMap,currentTexCoords).r;
		currentLayerDepth -=layerDepth;
		shadowFactor+=1.0f;
	}
	shadowFactor /=numLayers;
	
	return shadowFactor;
}
float ShadowCalculation(vec3 fragPos)
{
	vec4 fragToLight = vec4(fragPos - pointLight.position,0.0f);
	float currentDepth = CalculationShadowDepth(fragToLight.xyz);
	currentDepth = (currentDepth*0.5f) + 0.5f;
	fragToLight.x*=-1.0;
	float bias = 0.0001f;
	fragToLight = normalize(fragToLight);
	fragToLight.w = currentDepth - bias ;
	int samples  = 20;
	vec2 size = textureSize(depthMap,0);
	float diskRadius = 1.0f/size.x;
	//diskRadius = 0.002
	float shadow=0.0f;
	
	for(int i=0;i<20;++i)
	{
		shadow += texture(depthMap, fragToLight + vec4(sampleOffsetDirections[i]* (diskRadius*2.0f),0.0f));
	}
	shadow /=20.0f;
	return shadow;
}
// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewDir);

vec2 ParallaxMapping(vec2 texCoords,vec3 viewDir)
{
	//normal ParallaxMapping
	//float height = texture(heightMap,texCoords).r;
	//vec2 p = viewDir.xy/viewDir.z * (height * height_scale);
	//return texCoords - p;

	// Steep ParrallaxMapping
	float numLayers =10;
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0)); 

	float layerDepth = 1.0f/numLayers;
	float currentLayerDepth = 0.0f;
	vec2 P = viewDir.xy * height_scale;
	vec2 deltaTexCoords = P / numLayers;

	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(heightMap,currentTexCoords).r;

	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(heightMap,currentTexCoords).r;
		currentLayerDepth +=layerDepth;
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(heightMap,prevTexCoords).r - currentLayerDepth + layerDepth;

	//interpolation
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	return finalTexCoords;
}
void main()
{
	vec4 texcolor = texture(material.diffuse,TexCoords);
	if(texcolor.a<0.1)
		discard;
	vec3 norm;
	vec3 viewDir = normalize(viewPosition - FragPos);
	vec3 tanViewDir = normalize(TBN*viewDir);

	vec2 texCoords = TexCoords;
	texCoords = ParallaxMapping(TexCoords,tanViewDir);
	if(texCoords.x >1.0 || texCoords.y >1.0||texCoords.x<0.0||texCoords.y <0.0)
	 discard;
	if(!material.normalMap)
	{
	   norm = normalize(v_Normal);
	}
	else
	{
		norm = texture(material.normal,texCoords).rgb;
		norm.r = norm.r *2.0 - 1.0;
		norm.g = norm.g *2.0 - 1.0;
		norm.b = norm.b *2.0 - 1.0;
		norm = normalize(TBN * norm);
	}	
	
	vec3 result;
	// dir light
	//result +=CalcDirLight(dirLight,norm,viewDir);
	 //point light
	result +=CalcPointLight(pointLight,texCoords,norm,FragPos,viewDir);

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
vec3 CalcPointLight(PointLight light,vec2 texCoords, vec3 normal, vec3 fragPos, vec3 viewDir)
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
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	float shadow = ShadowCalculation(fragPos);
	//shadow = 1.0f;
	float shadowFactor = 1.0f;
	//shadowFactor = ParallaxShadow(lightDir,texCoords);
    return (ambient +(shadow * shadowFactor)* (diffuse + specular));
}
