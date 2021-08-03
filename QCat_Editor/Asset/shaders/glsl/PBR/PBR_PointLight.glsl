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
layout(std140,binding = 4) uniform LightTransform
{
	mat4 LightMat[5];
	float cascadeEndClipSpace[6];
};
struct VertexOutput
{
    vec2 TexCoords;
	vec3 v_Normal;
	vec3 FragPos;
	mat3 TBN;
	float clipspaceZ;
};
layout(location = 0 ) out VertexOutput Output;

void main()
{
	Output.TexCoords = a_TexCoord;
	mat3 normalMatrix = mat3(transpose(u_InvTransform));
	Output.v_Normal =normalMatrix * a_Normal;
	gl_Position = u_Projection*u_View*u_Transform * vec4(a_Position, 1.0);
	
	//clipSpace Position Z
	Output.clipspaceZ = gl_Position.z;
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

	int IsAlbedoMap;
	int IsNormalMap;
	int IsMetallicMap;
	int IsRoughnessMap;
	int IsAoMap;
};
struct PointLight
{
	vec3 position;
	vec3 diffuse;
	float isActive;
};
struct DirLight
{	
	vec3 lightDirection;
	vec3 diffuse;
	float isActive;
	bool isDebug;
	bool isSoft;
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
	PointLight pointLight[4];
	DirLight	dirLight;
};
layout(std140,binding = 4) uniform LightTransform
{
	mat4 LightMat[5];
	float cascadeEndClipSpace[6];
};
layout(location = 0) out vec4 color;

struct VertexOutput
{
    vec2 TexCoords;
	vec3 v_Normal;
	vec3 FragPos;
	mat3 TBN;
	float clipspaceZ;
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

//cascadeShadowMap
layout (binding = 8) uniform sampler2DArrayShadow DirshadowMap;
layout (binding = 9) uniform sampler2DArray dirshadowMapHarsh;

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

// calculate ShadowFactor
float CalcShadowFactor(int cascadeIndex , vec4 LightSpacePos,bool isSoft)
{
	vec3 projCoords = LightSpacePos.xyz /LightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	if(projCoords.z>1.0)
		return 0.0;
	float currentDepth = projCoords.z;
	float bias = 0.01f;
	float comparisonResult = 0.0;
	if(isSoft)
	{
		vec2 texelSize = textureSize(DirshadowMap,0).xy;
		texelSize  = 1.0f/texelSize;
		for(int x = -1;x<=1;++x)
		{
			for(int y=-1;y<=1;++y)
			{
				vec4 coords = vec4(projCoords.xy + vec2(x,y) * texelSize.xy,cascadeIndex,currentDepth-bias);
				comparisonResult += texture(DirshadowMap,coords);
			}
		}
		comparisonResult /=9.0;
	}
	else
	{
		vec3 coords = vec3(projCoords.xy ,cascadeIndex);
		float result = texture(dirshadowMapHarsh,coords).r;
		if(result< currentDepth - bias)
			comparisonResult = 0.0f;
		else
			comparisonResult = 1.0f;
	}
	return comparisonResult;
}
bool IsInTexSpace(vec4 texpos)
{
	if(0<=texpos.x && texpos.x <=1.0 && 0<=texpos.y && texpos.y<=1.0&& 0<=texpos.z && texpos.z<=1.0)
	{
		return true;
	}
	else
		return false;
}
vec3 CalculateDirectLight(vec3 normal,vec3 viewDir,vec3 lightDir,vec3 diffuse,float attenuation,float roughness,vec3 F0,vec3 albedo,float metallic)
{
	vec3 V = viewDir;
	vec3 N = normal;
	vec3 Lo;
	// calculate per-light radiance
    vec3 L = lightDir;
    vec3 H = normalize(V + L);
  
    vec3 radiance = diffuse * attenuation;

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
	return Lo;
}
void main()
{
	vec3 albedo = material.albedo * (1.0- material.IsAlbedoMap) + pow(texture(albedoMap, Input.TexCoords).rgb,vec3(2.2)) * material.IsAlbedoMap;
	//vec3 albedo = material.IsAlbedoMap ? pow(texture(albedoMap, Input.TexCoords).rgb,vec3(2.2)) : material.albedo;
	float metallic = material.metallic * (1.0-material.IsMetallicMap) +  texture(metallicMap, Input.TexCoords).r *material.IsMetallicMap;
	float roughness =  material.roughness * (1.0-material.IsRoughnessMap) +  texture(roughnessMap, Input.TexCoords).r * material.IsRoughnessMap;
	float ao = material.ambientocclusion *(1.0-material.IsAoMap)  +  texture(aoMap, Input.TexCoords).r *material.IsAoMap;

	//float metallic = material.IsMetallicMap ? texture(metallicMap, Input.TexCoords).r : material.metallic;
	//float roughness = material.IsRoughnessMap ? texture(roughnessMap, Input.TexCoords).r : material.roughness;
	//float ao = material.IsAoMap ? texture(aoMap, Input.TexCoords).r : material.ambientocclusion;

	vec3 normal = normalize(Input.v_Normal);
	vec3 mapNormal = texture(normalMap,Input.TexCoords).rgb;
	mapNormal.r = mapNormal.r *2.0 - 1.0;
	mapNormal.g = mapNormal.g *2.0 - 1.0;
	mapNormal.b = mapNormal.b *2.0 - 1.0;
	mapNormal = normalize(Input.TBN * mapNormal);

	vec3 N;
	vec3 V;
	V =  normalize(u_viewPosition - Input.FragPos);
	N = normal * (1.0f - material.IsNormalMap) + mapNormal * material.IsNormalMap;
	vec3 R = reflect(-V, N); 

	vec3 result = vec3(0.0f);
	vec3 Lo = vec3(0.0);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0,albedo,metallic);

	float shadowFactor=1.0f;
	vec3 checkcolor[5];
	checkcolor[0] = vec3(1.0,0.0,0.0);
	checkcolor[1] = vec3(0.0,1.0,0.0);
	checkcolor[2] = vec3(0.0,0.0,1.0);
	checkcolor[3]= vec3(1.0,1.0,0.0);
	checkcolor[4] = vec3(0.0,1.0,1.0);
	vec3 debugColor = vec3(0.0);

	//DirLight
	for(int i=0;i<1;++i)
	{
		Lo +=CalculateDirectLight(N,V,-dirLight.lightDirection,dirLight.diffuse,1.0f,roughness,F0,albedo,metallic) * dirLight.isActive;
		
		for (int j = 0; j < 5 ; j++) 
		{
			if (Input.clipspaceZ <= cascadeEndClipSpace[j+1]) 
			{
				debugColor =checkcolor[j];
				vec4 lightSpacePos = LightMat[j] * vec4(Input.FragPos,1.0f);

				float nearZ =cascadeEndClipSpace[j];
				float farZ =cascadeEndClipSpace[j+1];
				//20% edge
				float cascadeEdge = (farZ - nearZ) * 0.2;
				float csmx = farZ - cascadeEdge;
				float shadowMain =1.0f;
				if(Input.clipspaceZ >= nearZ && Input.clipspaceZ <= farZ)
				{
					shadowMain = CalcShadowFactor(j,lightSpacePos,dirLight.isSoft);
					float shadowFallback = 1.0f;
					if(Input.clipspaceZ >= csmx)
					{
						float ratio = (Input.clipspaceZ - csmx) / (farZ - csmx);
						if(j<4)
						{
							lightSpacePos = LightMat[j+1] * vec4(Input.FragPos,1.0f);
							shadowFallback = CalcShadowFactor(j+1,lightSpacePos,dirLight.isSoft);
						}
						shadowMain = mix(shadowMain , shadowFallback , ratio);						
					}
				 }
				shadowFactor = shadowMain;
				break;   
			}
		}
		if(dirLight.isActive <1.0f)
			shadowFactor = 1.0f;
	}
	//PointLight
	for(int i=0;i<4;++i)
	{
        vec3 L = normalize(pointLight[i].position - Input.FragPos);
		float distance = length(pointLight[i].position - Input.FragPos);
		float attenuation = 1.0 / (distance * distance);
		Lo +=CalculateDirectLight(N,V,L,pointLight[i].diffuse,attenuation,roughness,F0,albedo,metallic) *pointLight[i].isActive;
	}

	// indirect part
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

	
	//Lo = mix(vec3(0.0f),Lo,shadowFactor);
	//Lo = Lo +  * shadowFactor;
	if(dirLight.isDebug)
		Lo +=debugColor;
	result = (ambient + (Lo)*shadowFactor);


	//HDR toneMapping
	result = result / (result + vec3(1.0));
	result = pow(result,vec3(1.0/2.2));

	color = vec4(result,1.0f);
}
