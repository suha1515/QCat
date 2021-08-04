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
	mat4 DirLightMat[5];
	mat4 SpotLightMat[8];
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
	float farPlane;
	float nearPlane;
	bool isActive;
};
struct DirLight
{	
	vec3 lightDirection;
	vec3 diffuse;
	bool isActive;
};
struct SpotLight
{
	vec3 position;
	vec3 lightDirection;
	vec3 diffuse;
	float farPlane;

	float cutOff;
	float outerCutoff;
	bool isActive;
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
	SpotLight spotLight[8];
	DirLight	dirLight;
	bool isSoft;
	bool isDebug;
};
layout(std140,binding = 4) uniform LightTransform
{
	mat4 DirLightMat[5];
	mat4 SpotLightMat[8];
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
layout (binding = 10) uniform samplerCubeArrayShadow pointShadowMap;
layout (binding = 11) uniform samplerCubeArray _pointShadowMap;
layout (binding = 12) uniform sampler2DArrayShadow spotShadowMap;
layout (binding = 13) uniform sampler2DArray _spotShadowMap;

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
bool IsInTexSpace(vec3 texpos)
{
	if(0<=texpos.x && texpos.x <=1.0 && 0<=texpos.y && texpos.y<=1.0&& 0<=texpos.z && texpos.z<=1.0)
	{
		return true;
	}
	else
		return false;
}
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);
// point light shadow Depth
float CalculationShadowDepth(vec3 shadowPos,float far_plane,float near_plane)
{
	// -1 t0 1 for z
	float c1 = (far_plane + near_plane) / (far_plane - near_plane);
	float c0 = -(2 * far_plane * near_plane) / (far_plane - near_plane);
	vec3 m = abs(shadowPos).xyz;
	float major = max(m.x, max(m.y, m.z));
	return (c1 * major + c0) / major;
}
// point light Shadow Calc
float PointShadowCalculation(int lightIndex,vec4 fragToLight,float far_plane,float near_plane,bool isSoft)
{
	float currentDepth = CalculationShadowDepth(fragToLight.xyz,far_plane,near_plane);
	currentDepth = (currentDepth*0.5f) + 0.5f;

	float bias = 0.01f;
	fragToLight = normalize(fragToLight);
	fragToLight.w = lightIndex;
	currentDepth -=bias;
	float shadow=0.0f;
	if(isSoft)
	{
		vec2 size = textureSize(pointShadowMap,0).xy;
		float diskRadius = 1.0f/size.x;
		//diskRadius = 0.002
		for(int i=0;i<20;++i)
		{
			vec4 coords = fragToLight + vec4(sampleOffsetDirections[i]* (diskRadius*2.0f),0.0f);
			shadow += texture(pointShadowMap,coords,currentDepth );
		}
		shadow /=20.0f;
	}
	else
	{
		 shadow = texture(_pointShadowMap, fragToLight).r;
		 if(shadow< currentDepth)
			shadow = 0.0f;
		else
			shadow = 1.0f;
	}
	
	return shadow;
}

// calculate DirShadowFactor
float CalcPCFShadowFactor(int lightIndex , vec4 LightSpacePos,sampler2DArrayShadow shadowMap)
{
	vec3 projCoords = LightSpacePos.xyz /LightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	if(!IsInTexSpace(projCoords))
		return 0.0;
	float currentDepth = projCoords.z;
	float bias = 0.001f;
	float comparisonResult = 0.0;
	vec2 texelSize = textureSize(shadowMap,0).xy;
	texelSize  = 1.0f/texelSize;
	for(int x = -1;x<=1;++x)
	{
		for(int y=-1;y<=1;++y)
		{
			vec4 coords = vec4(projCoords.xy + vec2(x,y) * texelSize.xy,lightIndex,currentDepth-bias);
			comparisonResult += texture(shadowMap,coords);
		}
	}
	comparisonResult /=9.0;
	return comparisonResult;
}
float CalcDefaultShadowFactor(int lightIndex , vec4 LightSpacePos,sampler2DArray shadowMap)
{
	vec3 projCoords = LightSpacePos.xyz /LightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;
	if(!IsInTexSpace(projCoords))
		return 0.0;
	float currentDepth = projCoords.z;
	float bias = 0.01f;
	float comparisonResult = 0.0;

	vec3 coords = vec3(projCoords.xy ,lightIndex);
	float result = texture(shadowMap,coords).r;
	if(result< currentDepth - bias)
		comparisonResult = 0.0f;
	else
		comparisonResult = 1.0f;

	return comparisonResult;
}

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
vec3 CalculateDirectLight(vec3 normal,vec3 viewDir,vec3 lightDir,vec3 diffuse,float attenuation,float intensity,float roughness,vec3 F0,vec3 albedo,float metallic)
{
	vec3 V = viewDir;
	vec3 N = normal;
	vec3 Lo;
	// calculate per-light radiance
    vec3 L = lightDir;
    vec3 H = normalize(V + L) ;
	
    vec3 radiance = diffuse * attenuation* intensity;

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
	if(dirLight.isActive)
	{	
		float shadowMain =1.0f;
		for (int j = 0; j < 5 ; j++) 
		{
			if (Input.clipspaceZ <= cascadeEndClipSpace[j+1]) 
			{
				debugColor =checkcolor[j];
				vec4 lightSpacePos = DirLightMat[j] * vec4(Input.FragPos,1.0f);

				float nearZ =cascadeEndClipSpace[j];
				float farZ =cascadeEndClipSpace[j+1];
				//20% edge
				float cascadeEdge = (farZ - nearZ) * 0.2;
				float csmx = farZ - cascadeEdge;
				if(Input.clipspaceZ >= nearZ && Input.clipspaceZ <= farZ)
				{
					shadowMain = isSoft ? CalcPCFShadowFactor(j,lightSpacePos,DirshadowMap) :  CalcDefaultShadowFactor(j, lightSpacePos,dirshadowMapHarsh);
					//shadowMain = CalcShadowFactor(j,lightSpacePos,isSoft);
					float shadowFallback = 1.0f;
					if(Input.clipspaceZ >= csmx)
					{
						float ratio = (Input.clipspaceZ - csmx) / (farZ - csmx);
						if(j<4)
						{
							lightSpacePos = DirLightMat[j+1] * vec4(Input.FragPos,1.0f);
							shadowFallback = isSoft ? CalcPCFShadowFactor(j+1,lightSpacePos,DirshadowMap) :  CalcDefaultShadowFactor(j+1, lightSpacePos,dirshadowMapHarsh);
						}
						shadowMain = mix(shadowMain , shadowFallback , ratio);						
					}
				 }
				break;   
			}
		}
		Lo +=CalculateDirectLight(N,V,-dirLight.lightDirection,dirLight.diffuse,1.0f,1.0f,roughness,F0,albedo,metallic) * shadowMain;
	}
	//PointLight
	for(int i=0;i<4;++i)
	{
		float distance = length(pointLight[i].position - Input.FragPos);
		if(pointLight[i].isActive&& distance <= pointLight[i].farPlane)
		{	
			vec3 L = normalize(pointLight[i].position - Input.FragPos);
			vec4 fragToLight = vec4(Input.FragPos - pointLight[i].position,0.0f);
			float attenuation = 1.0 / (distance * distance);
			float shadowMain =1.0f;
			shadowMain =  PointShadowCalculation(i,fragToLight,pointLight[i].farPlane,pointLight[i].nearPlane,isSoft);
			Lo +=CalculateDirectLight(N,V,L,pointLight[i].diffuse,attenuation,1.0f,roughness,F0,albedo,metallic) * shadowMain;
		}	
	}

	//SpotLight
	for(int i=0;i<8;++i)
	{
		if(spotLight[i].isActive)
		{
			vec3 L = normalize(spotLight[i].position - Input.FragPos);
			float distance = length(spotLight[i].position - Input.FragPos);
			vec4 lightSpacePos = SpotLightMat[i] * vec4(Input.FragPos,1.0f);
			float attenuation = 1.0 / (distance * distance);

			float theta = dot(L,-spotLight[i].lightDirection);
			float epsilon = spotLight[i].cutOff - spotLight[i].outerCutoff;
			float intensity = clamp((theta - spotLight[i].outerCutoff)/epsilon,0.0,1.0);
			float shadowMain =1.0f;

			float nearZ =cascadeEndClipSpace[4];
			float farZ =cascadeEndClipSpace[5];
			//20% edge
			float cascadeEdge = (farZ - nearZ) * 0.2;
			float csmx = farZ - cascadeEdge;
			if(Input.clipspaceZ <= farZ)
			{
				shadowMain =  isSoft ? CalcPCFShadowFactor(i,lightSpacePos,spotShadowMap) :  CalcDefaultShadowFactor(i, lightSpacePos,_spotShadowMap);
				shadowMain *= intensity;
				if(Input.clipspaceZ >= csmx)
				{
					float ratio = (Input.clipspaceZ - csmx) / (farZ - csmx);
					shadowMain = mix(shadowMain , 1.0f , ratio);
				}
			}		
			//shadowMain =  CalcPCFShadowFactor(i,lightSpacePos,spotShadowMap);
			
			Lo +=CalculateDirectLight(N,V,L,spotLight[i].diffuse,attenuation,intensity,roughness,F0,albedo,metallic) *shadowMain;

		}
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

	if(isDebug)
		Lo +=debugColor;
	result = (ambient + Lo);


	//HDR toneMapping
	result = result / (result + vec3(1.0));
	result = pow(result,vec3(1.0/2.2));

	color = vec4(result,1.0f);
}
