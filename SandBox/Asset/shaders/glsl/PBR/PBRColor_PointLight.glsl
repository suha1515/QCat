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
	v_Normal =normalMatrix * a_Normal;
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

	vec3 albedo;
	float metallic;
	float roughness;
	float ao;

	bool normalMap;
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

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform Material material;
uniform PointLight pointLight[4];
uniform vec3 u_viewPosition;

// function prototypes
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,Material material,vec3 F0);
const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------

void main()
{
	vec3 N;
	vec3 V;
	V =  normalize(u_viewPosition - FragPos);
	if(!material.normalMap)
	{
	   N = normalize(v_Normal);
	}
	else
	{
		N = texture(material.normal,TexCoords).rgb;
		N.r = N.r *2.0 - 1.0;
		N.g = N.g *2.0 - 1.0;
		N.b = N.b *2.0 - 1.0;
		N = normalize(TBN * N);
	}	
		
	vec3 result = vec3(0.0f);
	vec3 Lo = vec3(0.0);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0,material.albedo,material.metallic);
	
	for(int i=0;i<4;++i)
	{
		// calculate per-light radiance
        vec3 L = normalize(pointLight[i].position - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLight[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLight[i].diffuse * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, material.roughness);   
        float G   = GeometrySmith(N, V, L,  material.roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - material.metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * material.albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}

	vec3 ambient = vec3(0.03) * material.albedo * material.ao;

	result = ambient + Lo;

	//HDR toneMapping
	result = result / (result + vec3(1.0));
	result = pow(result,vec3(1.0/2.2));

	color = vec4(result,1.0f);

}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,Material material,vec3 F0)
{

    vec3 lightDir = normalize(light.position - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	// attenuation
    float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = light.diffuse * attenuation;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal,halfwayDir,material.roughness);
	float G = GeometrySmith(normal,viewDir,lightDir,material.roughness);
	vec3 F = fresnelSchlick(clamp(dot(halfwayDir,viewDir),0.0,1.0),F0);

	vec3 nominator = NDF* G * F;
	float denominator = 4 * max(dot(normal,viewDir),0.0) * max(dot(normal,lightDir),0.0);
	vec3 specular = nominator/max(denominator,0.001); // prevent devide by zero

	vec3 kS = F;
	vec3 kD = vec3(1.0f) - kS;

	kD *=1.0 - material.metallic;

	float NtoL = max(dot(normal,lightDir),0.0);
	
	return (kD * material.albedo / PI + specular) * radiance *NtoL;
}