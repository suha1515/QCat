struct PointLight
{
	float3 position;

	float constant;
	float Linear;
	float quadratic;
	float radius;

	float3 ambient;
	float3 diffuse;
	float3 specular;
};

struct PSIn
{
	float2 tc : Texcoord;
	float4 position : SV_POSITION;
};

const static int NUM_LIGHT = 1;

Texture2D gPosition;
Texture2D gNormal;
Texture2D aAlbedoSpec;
Texture2D ssao;

SamplerState posSplr: register(s0);
SamplerState normalSplr;
SamplerState albedoSplr;
SamplerState ssaoSplr;

cbuffer light : register(b0)
{
	PointLight pointLight[NUM_LIGHT];
}

float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos, float3 viewDir,float4 DiffuseSpec, float ambientOcclusion)
{
	float3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	float spec;

	float3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(0.0f, dot(normal, halfwayDir)), 64.0f);

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.Linear * distance + light.quadratic * (distance * distance));
	// combine results
	float3 ambient = light.ambient * DiffuseSpec.rgb * ambientOcclusion;
	float3 diffuse = light.diffuse * diff * DiffuseSpec.rgb;
	float3 specular = light.specular * spec * DiffuseSpec.a;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

float4 main(PSIn input) : SV_TARGET
{
	float3 FragPos = gPosition.Sample(posSplr,input.tc).rgb;
	float3 Normal = gNormal.Sample(normalSplr, input.tc).rgb;
	float4 DiffuseSpec = aAlbedoSpec.Sample(albedoSplr, input.tc);
	float AmbientOcclusion = ssao.Sample(ssaoSplr, input.tc).r;

	float3 result = float3(0.0f,0.0f,0.0f);
	float3 viewDir = normalize(-FragPos);
	[unroll]
	for (int i = 0; i < NUM_LIGHT; ++i)
	{
		//float distance = length(pointLight[i].position - FragPos);
		//if (distance < pointLight[i].radius)
			result += CalcPointLight(pointLight[i], Normal, FragPos, viewDir, DiffuseSpec, AmbientOcclusion);
	}


	return float4(result, 1.0f);
}