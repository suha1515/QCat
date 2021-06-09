// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_BitTangent;

layout(std140,binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
	vec3 viewPosition;
};
layout(std140,binding = 1) uniform Transform
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
layout(location = 0 ) out VertexOutput Output;

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
	vec3 albedo;
	float shininess;
	float metallic;
	float roughness;
	float ambientocclusion;

	bool IsAlbedoMap;
	bool IsNormalMap;
	bool IsMetallicMap;
	bool IsRoughnessMap;
	bool IsAoMap;
};
struct PointLight
{
	vec3 position;
	vec3 diffuse;
};
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
layout (binding = 2) uniform sampler2D metallicMap;
layout (binding = 3) uniform sampler2D roughnessMap;
layout (binding = 4) uniform sampler2D aoMap;
// IBL
layout (binding = 5) uniform samplerCube irradianceMap;
layout (binding = 6) uniform samplerCube prefilterMap;
layout (binding = 7) uniform sampler2D brdfLUT;

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
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}   
void main()
{
	vec3 albedo = material.IsAlbedoMap ? pow(texture(albedoMap, Input.TexCoords).rgb,vec3(2.2)) : material.albedo;
	float metallic = material.IsMetallicMap ? texture(metallicMap, Input.TexCoords).r : material.metallic;
	float roughness = material.IsRoughnessMap ? texture(roughnessMap, Input.TexCoords).r : material.roughness;
	float ao = material.IsAoMap ? texture(aoMap, Input.TexCoords).r : material.ambientocclusion;

	vec3 N;
	vec3 V;
	V =  normalize(u_viewPosition - Input.FragPos);
	if(!material.IsNormalMap)
	{
	   N = normalize(Input.v_Normal);
	}
	else
	{
		N = texture(normalMap,Input.TexCoords).rgb;
		N.r = N.r *2.0 - 1.0;
		N.g = N.g *2.0 - 1.0;
		N.b = N.b *2.0 - 1.0;
		N = normalize(Input.TBN * N);
	}	
	vec3 R = reflect(-V, N); 

	vec3 result = vec3(0.0f);
	vec3 Lo = vec3(0.0);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);
	
	for(int i=0;i<1;++i)
	{
		// calculate per-light radiance
        vec3 L = normalize(pointLight[i].position - Input.FragPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLight[i].position - Input.FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLight[i].diffuse * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L,  roughness);      
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
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}

	// ambient light (use ibl for ambient)
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD      *=1.0 - metallic;
	vec3 irradiance = texture(irradianceMap,N).rgb;
	vec3 diffuse = irradiance * albedo;

	// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	result = ambient + Lo;

	//HDR toneMapping
	result = result / (result + vec3(1.0));
	result = pow(result,vec3(1.0/2.2));

	color = vec4(result,1.0f);

}