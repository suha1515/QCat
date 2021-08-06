#type vertex
cbuffer Camera  : register(b0)
{
	matrix u_Projection;
	matrix u_View;
	float3 u_viewPosition;
}
cbuffer Transform : register(b1)
{
	matrix u_Transform;
	matrix u_invTransform;
}

struct VSOut
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
	float clipSpacePosZ : ClipPosZ;
};

struct VSIn
{
	float3 pos		: a_Position;
	float3 normal	: a_Normal;
	float2 tc		: a_Texcoord;
	float3 tan		: a_Tangent;
	float3 bitan	: a_BiTangent;
};
VSOut VSMain(VSIn Input)
{
	VSOut vso;
	matrix viewprojMat = mul(u_Projection, mul(u_View,u_Transform));
	float3x3 normalMat = (float3x3)transpose(u_invTransform);
	vso.pos = mul(viewprojMat, float4(Input.pos, 1.0f));
	vso.clipSpacePosZ = vso.pos.z;
	vso.tc = Input.tc;
	vso.normal = mul(normalMat, Input.normal);

	float3 T = normalize(mul(normalMat, Input.tan));
	float3 B = normalize(mul(normalMat, Input.bitan));
	float3 N = normalize(mul(normalMat, Input.normal));

	//T = normalize(T - dot(T, N) * N);

	float3x3 mat = { T,B,N };
	vso.TBN = mat;

	vso.TBN = transpose(vso.TBN);

	float3 FragPos = (float3)mul(u_Transform, float4(Input.pos, 1.0f));
	vso.fragPos = FragPos;
	vso.viewPosition = u_viewPosition;
	return vso;
}
#type pixel
struct DirLight
{
	float3 lightDirection;
	float3 diffuse;
	bool isActive;
};
struct PointLight
{
	float3 position;
	float3 diffuse;
	float farPlane;
	float nearPlane;
	bool isActive;
};
struct SpotLight
{
	float3 position;
	float3 lightDirection;
	float3 diffuse;
	float farPlane;

	float cutOff;
	float outerCutOff;
	bool isActive;
};
struct Material
{
	float3 albedo;
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
cbuffer material : register(b2)
{
	Material material;
}
cbuffer light : register(b3)
{
	PointLight pointLight[4];
	SpotLight spotLight[8];
	DirLight dirLight;
	bool isSoft;
	bool isDebug;
}
cbuffer LightTransform : register(b4)
{
	matrix DirLightMat[5];
	matrix SpotLightMat[8];
	float cascadeEndClipSpace[6];
}
Texture2D albedoMap : register(t0);
SamplerState  albedoMapSplr: register(s0);
Texture2D normalMap : register(t1);
SamplerState  normalMapSplr: register(s1);
Texture2D metallicMap: register(t2);
SamplerState  metallicMapSplr: register(s2);
Texture2D roughnessMap: register(t3);
SamplerState  roughnessMapSplr: register(s3);
Texture2D aoMap: register(t4);
SamplerState  aoMapSplr: register(s4);
//IBL
TextureCube irradianceMap: register(t5);;
SamplerState  irradianceMapSplr : register(s5);
TextureCube prefilterMap: register(t6);;
SamplerState  prefilterMapSplr: register(s6);
Texture2D brdfLUT: register(t7);
SamplerState  brdfLUTSplr: register(s7);

//Shadow Map
Texture2DArray cascadeShadowMap : register(t8);
TextureCubeArray pointShadowMap : register(t9);
Texture2DArray spotShadowMap : register(t10);
SamplerComparisonState shadowSplr : register(s8);
SamplerState  defaultSplr: register(s9);

struct PSIn
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
	float clipSpacePosZ : ClipPosZ;
};
struct PS_OUT
{
	float4 color :SV_TARGET0;
};
const static float3 gridSamplingDisk[20] = {
	{ 1, 1, 1}, { 1,-1, 1}, {-1,-1, 1}, {-1, 1, 1},
	{ 1, 1,-1}, { 1,-1,-1}, {-1,-1,-1}, {-1, 1,-1},
	{ 1, 1, 0}, { 1,-1, 0}, {-1,-1, 0}, {-1, 1, 0},
	{ 1, 0, 1}, {-1, 0, 1}, { 1, 0,-1}, {-1, 0,-1},
	{ 0, 1, 1}, { 0,-1, 1}, { 0,-1,-1}, { 0, 1,-1},
};

static const float PI = 3.14159265359;
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ---
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}


float CalcDepthInShadow(const in float3 fragPos,float far_plane,float near_plane)
{
	const float c1 = far_plane / (far_plane - near_plane);
	const float c0 = -near_plane * far_plane / (far_plane - near_plane);
	const float3 m = abs(fragPos).xyz;
	const float major = max(m.x, max(m.y, m.z));
	return (c1 * major + c0) / major;
}
float CalcPointShadowFactor(int index,TextureCubeArray cubeTex, float4 fragToLight, float far_plane, float near_plane, bool isSoft)
{
	float  currentDepth = CalcDepthInShadow(fragToLight.xyz, far_plane, near_plane);
	float width, height,element;
	cubeTex.GetDimensions(width, height,element);
	float textureSize = 1.0f / width;
	fragToLight = normalize(fragToLight);

	float bias = 0.01f;
	float shadow = 0.0f;
	float4 direction = float4(fragToLight + gridSamplingDisk[index] * (textureSize * 2),0.0f);
	direction.w = index;
	if (isSoft)
	{
		for (int i = 0; i < 20; ++i)
		{
			shadow += cubeTex.SampleCmpLevelZero(shadowSplr, direction, currentDepth - bias);
		}
		shadow /= 20.0f;
	}
	else
	{
		shadow = cubeTex.Sample(defaultSplr, direction).r;
		if (shadow < currentDepth - bias)
			shadow = 0.0f;
		else
			shadow = 1.0f;
	}
	return shadow;
}
float CalcDirSpotShadowFactor(int index, float4 lightSpacePos, Texture2DArray tex,bool isSoft)
{
	float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords.x = projCoords.x * 0.5 + 0.5f;
	projCoords.y = -projCoords.y * 0.5 + 0.5f;
	if (projCoords.z > 1.0)
		return 0.0f;

	float currentDepth = projCoords.z;
	float bias = 0.01f;
	float shadow = 0.0;

	float3 samplePos = projCoords;
	samplePos.z = index;

	if (isSoft)
	{
		[unroll]
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				shadow += tex.SampleCmpLevelZero(shadowSplr, samplePos, currentDepth - bias, int2(x, y));
			}
		}
		shadow /= 9.0f;
	}
	else
	{
		shadow = tex.Sample(defaultSplr, samplePos).r;
		if (shadow < currentDepth - bias)
			return 0.0f;
		else
			return 1.0f;
	}

	return shadow;
}
float CalcCascadeShadowFactor(int cascadeIndex, float4 lightspacepos,bool isSoft)
{
	float3 projCoords = lightspacepos.xyz / lightspacepos.w;
	projCoords.x = projCoords.x * 0.5 + 0.5f;
	projCoords.y = -projCoords.y * 0.5 + 0.5f;
	if (projCoords.z > 1.0)
		return 0.0f;

	float currentDepth = projCoords.z;
	float bias = 0.01f;
	float shadow = 0.0;

	float3 samplePos = projCoords;
	
	samplePos.z = cascadeIndex;
	if (isSoft)
	{
		[unroll]
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				shadow += cascadeShadowMap.SampleCmpLevelZero(shadowSplr, samplePos, currentDepth - bias, int2(x, y));
			}
		}
		shadow /= 9.0f;
	}
	else
	{
		shadow = cascadeShadowMap.Sample(defaultSplr, samplePos).r;
		if (shadow < currentDepth - bias)
			return 0.0f;
		else
			return 1.0f;
	}
	return shadow;
}

float3 CalculateDirectLight(float3 normal, float3 viewDir, float3 lightDir, float3 diffuse, float roughness, float metallic, float3 F0, float3 albedo, float attenuation,float intensity)
{
	float3 V = viewDir;
	float3 N = normal;
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	// Calc light Radiance
	float3 L = lightDir;
	float3 H = normalize(V + L);

	float3 radiance = diffuse * attenuation * intensity;

	// Cook - Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	float3 nominator = NDF * G * F;
	float denominator = 4 * max(0.0, dot(N, V)) * max(0.0, dot(N, L));
	float3 specular = nominator / max(0.001, denominator); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

	 // kS is equal to Fresnel
	float3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
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
PS_OUT PSMain(PSIn input)
{
	PS_OUT output;

	float3 albedo = material.albedo * (1 - material.IsAlbedoMap) + pow(albedoMap.Sample(albedoMapSplr, input.tc).rgb, 2.2f) * material.IsAlbedoMap;
	float metallic = material.metallic * (1-material.IsMetallicMap) + metallicMap.Sample(metallicMapSplr, input.tc).r * material.IsMetallicMap;
	float roughness = material.roughness * (1-material.IsRoughnessMap) + roughnessMap.Sample(roughnessMapSplr, input.tc).r * material.IsRoughnessMap;
	float ao = material.ambientocclusion * (1 - material.IsAoMap) + aoMap.Sample(aoMapSplr, input.tc).r * material.IsAoMap;

	//float3 albedo = material.IsAlbedoMap ? pow(albedoMap.Sample(albedoMapSplr, input.tc).rgb, 2.2f) : material.albedo;
	//float metallic = material.IsMetallicMap ? metallicMap.Sample(metallicMapSplr, input.tc).r : material.metallic;
	//float roughness = material.IsRoughnessMap ? roughnessMap.Sample(roughnessMapSplr, input.tc).r : material.roughness;
	//float ao = material.IsAoMap ? aoMap.Sample(aoMapSplr, input.tc).r : material.ambientocclusion;

	float4 color;
	float3 N;
	float3 V = normalize(input.viewPosition - input.fragPos);

	float3 normal = normalize(input.normal);
	float3 mapNormal = normalMap.Sample(normalMapSplr, input.tc).rgb;
	mapNormal = mapNormal * 2.0f - 1.0f;
	mapNormal = normalize(mul(input.TBN, mapNormal));

	N = normal * (1 - material.IsNormalMap) + mapNormal * material.IsNormalMap;

	float3 R = reflect(-V, N);

	float3 result = float3(0.0f, 0.0f, 0.0f);
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

	//Direct Light Part
	//Directional Light Calc
	float shadowFactor = 1.0f;
	float3 checkcolor[5];
	checkcolor[0] = float3(1.0, 0.0, 0.0);
	checkcolor[1] = float3(0.0, 1.0, 0.0);
	checkcolor[2] = float3(0.0, 0.0, 1.0);
	checkcolor[3] = float3(1.0, 1.0, 0.0);
	checkcolor[4] = float3(0.0, 1.0, 1.0);
	float3 debugColor = float3(0.0, 0.0, 0.0);

	if (dirLight.isActive)
	{
		float shadowMain = 1.0f;
		[unroll]
		for (int i = 0; i < 5; ++i)
		{
			float nearZ = cascadeEndClipSpace[i];
			float farZ = cascadeEndClipSpace[i + 1];
			float cascadeEdge = (farZ - nearZ) * 0.2;
			float csmx = farZ - cascadeEdge;
			if (input.clipSpacePosZ >= nearZ && input.clipSpacePosZ <= farZ)
			{
				float4 cascadeLightPos = mul(DirLightMat[i], float4(input.fragPos, 1.0f));
				shadowMain = CalcDirSpotShadowFactor(i, cascadeLightPos, cascadeShadowMap, isSoft);
				//shadowMain = CalcCascadeShadowFactor(i, cascadeLightPos,dirLight.isSoft);
				float shadowFallback = 1.0f;
				if (input.clipSpacePosZ >= csmx)
				{
					float ratio = (input.clipSpacePosZ - csmx) / (farZ - csmx);
					if (i < 4)
					{
						cascadeLightPos = mul(DirLightMat[i + 1],float4(input.fragPos, 1.0f));
						shadowFallback = CalcDirSpotShadowFactor(i+1, cascadeLightPos, cascadeShadowMap, isSoft);
					}
					shadowMain = lerp(shadowMain, shadowFallback, ratio);
				}
				shadowFactor = shadowMain;
				debugColor = checkcolor[i];
				break;
			}
		}
		Lo += CalculateDirectLight(N, V, -dirLight.lightDirection, dirLight.diffuse, roughness, metallic, F0, albedo, 1.0f, 1.0f) * shadowMain;
	}
	float nearZ = cascadeEndClipSpace[4];
	float farZ = cascadeEndClipSpace[5];
	//20% edge
	float cascadeEdge = (farZ - nearZ) * 0.2;
	float csmx = farZ - cascadeEdge;
	float ratio = (input.clipSpacePosZ - csmx) / (farZ - csmx);
	ratio = clamp(ratio, 0.0, 1.0);

	// Point Light Calc
	[unroll]
	for (int k = 0; k < 4; ++k)
	{
		float distance = length(pointLight[k].position - input.fragPos);
		if (pointLight[k].isActive)
		{
			float3 L = normalize(pointLight[k].position - input.fragPos);
			float4 fragToLight = float4(input.fragPos - pointLight[k].position, 0.0f);
			float attenuation = 1.0 / (distance * distance);
			float shadowMain = 1.0f;
			shadowMain = CalcPointShadowFactor(k, pointShadowMap, fragToLight, pointLight[k].farPlane, pointLight[k].nearPlane, isSoft);
			shadowMain = lerp(shadowMain, 1.0f, ratio);

			Lo += CalculateDirectLight(N, V, L, pointLight[k].diffuse, roughness, metallic, F0, albedo, attenuation,1.0f)*shadowMain;
		}
	}

	//SpotLight Calc
	[unroll]
	for (int j = 0; j < 8; ++j)
	{
		if (spotLight[j].isActive)
		{
			float3 L = normalize(spotLight[j].position - input.fragPos);
			float distance = length(spotLight[j].position - input.fragPos);
			float4 lightSpacePos = mul(SpotLightMat[j], float4(input.fragPos, 1.0f));
			float attenuation = 1.0 / (distance * distance);

			
			float theta = dot(L, -spotLight[j].lightDirection);
			float epsilon = spotLight[j].cutOff - spotLight[j].outerCutOff;
			float intensity = clamp((theta - spotLight[j].outerCutOff) / epsilon, 0.0, 1.0);
			float shadowMain = 1.0f;

			shadowMain = CalcDirSpotShadowFactor(j, lightSpacePos, spotShadowMap, isSoft);
			shadowMain *= intensity;
			shadowMain = lerp(shadowMain, 1.0f, ratio),0.0,1.0;

			Lo += CalculateDirectLight(N, V, L, spotLight[j].diffuse, roughness, metallic, F0, albedo, attenuation, intensity) * shadowMain;
		}
	}

	//InDirectLight Part
	//ambient light
	float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - metallic;
	float3 irradiance = irradianceMap.Sample(irradianceMapSplr, N).rgb;
	float3 diffuse = irradiance * albedo;

	const float MAX_REFLECTION_LOD = 4.0;
	float3 prefilteredColor = prefilterMap.SampleLevel(prefilterMapSplr, R, roughness * MAX_REFLECTION_LOD).rgb;
	float2 brdf = brdfLUT.Sample(brdfLUTSplr, float2(max(dot(N, V), 0.0), roughness)).rg;
	float3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	float3 ambient = (kD * diffuse + specular) * ao;

	if (isDebug)
		Lo += debugColor;

	result = ambient + Lo;
	//HDR tonemapping
	result = result / (result + float3(1.0f, 1.0f, 1.0f));
	result = pow(result, 1.0f / 2.2f);
	
	output.color = float4(result, 1.0f);
	return output;
}