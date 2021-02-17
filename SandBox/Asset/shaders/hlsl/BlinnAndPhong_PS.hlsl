
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
	bool	blinn;
	bool	gamma;
}
cbuffer material : register(b1)
{
	Material material;
}
Texture2D diffuseTex;
Texture2D specularTex;
Texture2D normalMapTex;

SamplerState splr : register(s0);

struct PS_OUT
{
	float4 color :SV_TARGET0;
};

// calculates the color when using a directional light.
float3 CalcDirLight(DirLight light, float3 normal, float3 viewDir, float2 tc , Texture2D diffuseTex,Texture2D specularTex)
{
	float3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	float3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(0.0f,dot(viewDir, reflectDir)), material.shininess);
	// combine results
	float3 ambient = light.ambient * diffuseTex.Sample(splr,tc).rgb;
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;
	return (ambient + diffuse + specular);
}
// calculates the color when using a point light.
float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos,float3 lightDir, float3 viewDir, float2 tc, Texture2D diffuseTex, Texture2D specularTex)
{
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	float spec;

	if (blinn)
	{
		float3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(0.0f, dot(normal, halfwayDir)), material.shininess * 4);
	}
	else
	{
		// specular shading
		float3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(0.0f, dot(viewDir, reflectDir)), material.shininess);
	}
	
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));
	// combine results
	float3 ambient = light.ambient * diffuseTex.Sample(splr, tc).rgb;
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
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
	float2 tc			   : Texcoord;
	float3 normal		   :  Normal;
	float3 fragPos		   : FragPos;
	float3 tanFragPosition : tanFragPos;
	float3 tanViewPosition : tanViewPos;
	float3 tanLightPosition: tanLightPos;
	float3 viewPosition    : ViewPos;
	float4 pos :SV_Position;
};
PS_OUT main(PSIn input)
{
	PS_OUT output;
	float4 color;
	float3 norm;
	float3 lightDir;
	float3 viewDir;
	float3 fragPos;
	if (material.normalMap)
	{
		norm = normalMapTex.Sample(splr, input.tc).rgb;
		norm = norm * 2.0f - 1.0f;
		norm = normalize(norm);
		lightDir = normalize(input.tanLightPosition - input.tanFragPosition);
		viewDir = normalize(input.tanViewPosition - input.tanFragPosition);
		fragPos = input.tanFragPosition;
	}
	else
	{
		norm = normalize(input.normal);
		lightDir = normalize(pointLight.position - input.fragPos);
		viewDir = normalize(input.viewPosition - input.fragPos);
		fragPos = input.fragPos;
	}
	
	float3 result = float3(0.0f,0.0f,0.0f);

	float4 texcolor = diffuseTex.Sample(splr, input.tc).rgba;
	clip(texcolor.a < 0.1f ? -1 : 1);
	// point light
	result += CalcPointLight(pointLight, norm, fragPos,lightDir, viewDir, input.tc, diffuseTex, specularTex);
	if (gamma)
		result = pow(result,1.0f / 2.2f);

	output.color = float4(result, texcolor.a);
	return output;
}