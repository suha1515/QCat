
struct Light
{
	float3 position;
	float3 ambient;
	float3 diffuse;
	float3 specular;
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
Texture2D emissionTex;

SamplerState splr : register(s0);

float4 main(float2 tc: Texcoord, float3 normal : Normal, float3 fragPos : FragPos) : SV_TARGET
{
	//ambient
	float3 ambient = light.ambient * diffuseTex.Sample(splr,tc).rgb;

	// diffuse
	float3 norm = normalize(normal);
	float3 lightDir = normalize(light.position - fragPos);
	float diff = max(0.0f,dot(norm, lightDir));
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr,tc).rgb;

	//specular
	float3 viewDir = normalize(viewPosition - fragPos);
	float3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(0.0f, dot(viewDir, reflectDir)), material.shininess);
	float3 specular = light.specular * spec * specularTex.Sample(splr,tc).rgb;

	float3 emission = emissionTex.Sample(splr, tc).rgb;

	return float4(diffuse + ambient + specular + emission,1.0f) ;
}