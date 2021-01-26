struct Test2
{
	float b;
};
struct Test
{
	float a;
	float b;
	float array[2];
	Test2 test[3];
};
struct Light
{
	float3 position;
	float3 ambient;
	float3 diffuse;
	float3 specular;
};
struct Material
{
	float3 diffuse;
	float3 ambient;
	float3 specular;
	float shininess;
};
cbuffer light : register(b0)
{
	Light light;
	float b[3];
}
cbuffer material : register(b1)
{
	Material material;
	float3 viewPosition;
}

Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(float2 tc: Texcoord, float3 normal : Normal, float3 fragPos : FragPos) : SV_TARGET
{
	//ambient
	float3 ambient = light.ambient * material.ambient;

	// diffuse
	float3 norm = normalize(normal);
	float3 lightDir = normalize(light.position - fragPos);
	float diff = max(0.0f,dot(norm, lightDir));
	float3 diffuse = light.diffuse * (diff * material.diffuse);

	//specular
	float3 viewDir = normalize(viewPosition - fragPos);
	float3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(0.0f, dot(viewDir, reflectDir)), material.shininess);
	float3 specular = light.specular * (spec * material.specular);

	return tex.Sample(splr, tc)*float4(diffuse + ambient + specular,1.0f) ;
}