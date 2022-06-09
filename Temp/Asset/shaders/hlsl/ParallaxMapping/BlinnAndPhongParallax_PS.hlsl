
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
	bool	gamma;
}
cbuffer material : register(b1)
{
	Material material;
	float height_scale;
}
Texture2D diffuseTex;
Texture2D specularTex;
Texture2D normalMapTex;
Texture2D heightMap : register(t4);

SamplerState splr : register(s0);
struct PS_OUT
{
	float4 color :SV_TARGET0;
};
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
float3 CalcPointLight(PointLight light, float2 tc, float3 normal, float3 fragPos,float3 viewDir , Texture2D diffuseTex, Texture2D specularTex)
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
	// combine results
	float3 ambient = light.ambient * diffuseTex.Sample(splr, tc).rgb;
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
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
float2 ParallaxMapping(float2 texCoords, float3 viewDir)
{
	float numLayers = 10;
	const float minLayers = 8.0f;
	const float maxLayers = 32.0f;
	numLayers = lerp(maxLayers, minLayers, max(dot(float3(0.0f, 0.0f, 1.0f), viewDir), 0.0f));

	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f;
	float2 P = viewDir.xy * height_scale;
	float2 deltaTexCoords = P / numLayers;

	float2 currentTexCoords = texCoords;
	float currentDepthMapValue = heightMap.Sample(splr, currentTexCoords).r;

	[loop]
	while (currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = heightMap.Sample(splr, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}
	
	float2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = heightMap.Sample(splr, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation
	float weight = afterDepth / (afterDepth - beforeDepth);
	float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	return finalTexCoords;
}
PS_OUT main(PSIn input)
{
	float4 texcolor = diffuseTex.Sample(splr, input.tc).rgba;
	clip(texcolor.a < 0.1f ? -1 : 1);

	PS_OUT output;
	float4 color;
	float3 norm;
	float3 viewDir = normalize(input.viewPosition - input.fragPos);
	float3 tanViewDir = normalize(mul(input.TBN, viewDir));

	float2 texCoords = input.tc;
	texCoords = ParallaxMapping(texCoords, tanViewDir);
	clip(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0 ? -1 : 1);
	if (material.normalMap)
	{
		norm = normalMapTex.Sample(splr, texCoords).rgb;
		norm = norm * 2.0f - 1.0f;
		norm = normalize(mul(input.TBN,norm));
	}
	else
	{
		norm = normalize(input.normal);
	}
	
	float3 result = float3(0.0f,0.0f,0.0f);

	
	// dirLight
	//result += CalcDirLight(dirLight, norm,viewDir,input.tc,input.fragPosLightSpace,diffuseTex, specularTex);
	// point light
	result += CalcPointLight(pointLight,texCoords, norm, input.fragPos, viewDir, diffuseTex, specularTex);
	if (gamma)
		result = pow(result,1.0f / 2.2f);

	output.color = float4(result, texcolor.a);
	return output;
}