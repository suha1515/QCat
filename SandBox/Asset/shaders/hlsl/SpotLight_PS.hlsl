
struct Light
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
};
cbuffer light : register(b0)
{
	Light light;
}
cbuffer material : register(b1)
{
	Material material;
	float3 viewPosition;
}
Texture2D diffuseTex;
Texture2D specularTex;

SamplerState splr : register(s0);

float4 main(float2 tc: Texcoord, float3 normal : Normal, float3 fragPos : FragPos) : SV_TARGET
{
	float4 color;

	//ambient
	float3 ambient = light.ambient * diffuseTex.Sample(splr, tc).rgb;

	// diffuse
	float3 norm = normalize(normal);
	float3 lightDir = normalize(light.position - fragPos);
	float diff = max(0.0f, dot(norm, lightDir));
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;

	//specular
	float3 viewDir = normalize(viewPosition - fragPos);
	float3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(0.0f, dot(viewDir, reflectDir)), material.shininess);
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;


	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	diffuse *= intensity;
	specular *= intensity;
	//float3 emission = emissionTex.Sample(splr, tc).rgb;

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));


	//ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	color = float4(diffuse + ambient + specular, 1.0f);

	return color;
}