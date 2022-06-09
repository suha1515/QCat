
struct DirLight
{
	float3 direction;
	float3 diffuse;
	float3 ambient;
	float3 specular;
};
struct PointLight
{
	float3 position;

	float constant;
	float Linear;
	float quadratic;

	float3 ambient;
	float3 diffuse;
	float3 specular;
};

struct SpotLight
{
	float3 position;
	float3 direction;
	float3 ambient;
	float3 diffuse;
	float3 specular;

	float constant;
	float Linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};
struct Material
{
	float shininess;
	bool normalMap;
};
cbuffer light : register(b0)
{
	PointLight pointLight;
	DirLight   dirLight;
	float far_plane;
	float near_plane;
	bool	gamma;
}
cbuffer material : register(b1)
{
	Material material;
}
Texture2D diffuseTex;
Texture2D specularTex;
Texture2D normalMapTex;
TextureCube shadowMapTex : register(t4);

SamplerState splr : register(s0);
SamplerComparisonState shadowSampler : register(s4);
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
//shadow
float CalculationShadowDepth(const in float3 shadowPos)
{
	const float c1 = far_plane / (far_plane - near_plane);
	const float c0 = -near_plane * far_plane / (far_plane - near_plane);
	const float3 m = abs(shadowPos).xyz;
	const float major = max(m.x, max(m.y, m.z));
	return (c1 * major + c0) / major;
}
float ShadowCalculation(float3 fragPos,float3 lightPosition)
{
	float3 fragToLight = fragPos - lightPosition;
	float currentDepth = CalculationShadowDepth(fragToLight);
	float width;
	shadowMapTex.GetDimensions(width, width);
	float textureSize = 1.0f / width;
	fragToLight = normalize(fragToLight);
	//float closestDepth = shadowMapTex.Sample(splr, fragToLight).r;
	//closestDepth *= far_plane;
	//float currentDepth = length(fragToLight)/far_plane;
	float bias = 0.00001f;
	float shadow = 0.0f;
	
	//shadow = currentDepth - bias > closestDepth ? 0.0f : 1.0f;
	
	for (int i = 0; i < 20; ++i)
	{
		float3 dir = fragToLight + gridSamplingDisk[i]* (textureSize*2);
		shadow += shadowMapTex.SampleCmpLevelZero(shadowSampler, dir, currentDepth - bias);
	}
	shadow /= 20.0f;
	//shadow = shadowMapTex.SampleCmpLevelZero(shadowSampler, fragToLight, currentDepth - bias);
	return shadow;
}

// calculates the color when using a directional light.
float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 tc ,float4 fragPosLight, Texture2D diffuseTex,Texture2D specularTex)
{
	float3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	float spec;
	float3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(0.0f, dot(normal, halfwayDir)), material.shininess * 4);

	
	// combine results
	float3 ambient = light.ambient * diffuseTex.Sample(splr,tc).rgb;
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;
	//return (ambient + ((1.0f-shadow) *(diffuse + specular)));
	return (ambient + (diffuse + specular));

}
// calculates the color when using a point light.
float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos,float3 viewDir, float2 tc, Texture2D diffuseTex, Texture2D specularTex)
{
	float3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	float spec;

	float3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(0.0f, dot(normal, halfwayDir)), material.shininess * 4);
	
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));
	float shadow = ShadowCalculation(fragPos,light.position);
	// combine results
	float3 ambient = light.ambient * diffuseTex.Sample(splr, tc).rgb;
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + (shadow) * (diffuse + specular));
}
// calculates the color when using a spot light.
float3 CalcSpotLight(SpotLight light, float3 normal, float3 fragPos, float3 viewDir, float2 tc, Texture2D diffuseTex, Texture2D specularTex)
{
	float3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	float3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(0.0f, dot(viewDir, reflectDir)), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));
	// spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	float3 ambient = light.ambient * diffuseTex.Sample(splr, tc).rgb;
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}
struct PSIn
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
};
PS_OUT main(PSIn input)
{
	PS_OUT output;
	float4 color;
	float3 norm;
	float3 viewDir = normalize(input.viewPosition - input.fragPos);
	if (material.normalMap)
	{
		norm = normalMapTex.Sample(splr, input.tc).rgb;
		norm = norm * 2.0f - 1.0f;
		norm = normalize(mul(input.TBN,norm));
	}
	else
	{
		norm = normalize(input.normal);
	}
	
	float3 result = float3(0.0f,0.0f,0.0f);

	float4 texcolor = diffuseTex.Sample(splr, input.tc).rgba;
	clip(texcolor.a < 0.1f ? -1 : 1);
	// dirLight
	//result += CalcDirLight(dirLight, norm,viewDir,input.tc,input.fragPosLightSpace,diffuseTex, specularTex);
	// point light
	result += CalcPointLight(pointLight, norm, input.fragPos, viewDir, input.tc, diffuseTex, specularTex);
	if (gamma)
		result = pow(result,1.0f / 2.2f);

	output.color = float4(result, texcolor.a);
	return output;
}