#type vertex
cbuffer Camera : register(b0)
{
	matrix u_Projection;
	matrix u_View;
	float3 u_viewPosition;
}
cbuffer transform  : register(b1)
{
	matrix u_Transform;
	matrix u_invTransform;
	matrix u_BoneMatrices[100];
}
struct VSOut
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
};
struct VSIn
{
	float3 pos		: a_Position;
	float3 normal	: a_Normal;
	float2 tc		: a_Texcoord;
	float3 tan		: a_Tangent;
	float3 bitan	: a_BiTangent;
	int4 boneIDs	: a_BoneIDs;
	float4 weights  : a_Weights;
};
VSOut VSMain(VSIn Input)
{
	float4 totalPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 totalNormal = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		if (Input.boneIDs[i] == -1)
			continue;
		if (Input.boneIDs[i] >= 100)
		{
			totalPosition = float4(Input.pos, 1.0f);
			break;
		}
		float4 localPosition = mul(u_BoneMatrices[Input.boneIDs[i]],float4(Input.pos, 1.0f));
		totalPosition += localPosition * Input.weights[i];
		float3 localNormal = mul((float3x3)u_BoneMatrices[Input.boneIDs[i]], Input.normal);
		totalNormal += localNormal * Input.weights[i];
	}

	VSOut vso;

	matrix pvtMatrix = mul(mul(u_Projection, u_View),u_Transform);
	matrix projViewMatrix = mul(u_Projection, u_View);
	float3x3 normalMat = (float3x3)transpose(u_invTransform);
	vso.pos = mul(pvtMatrix, totalPosition);
	vso.tc = Input.tc;
	vso.normal = mul(normalMat, totalNormal);

	float3 T = normalize(mul(normalMat, Input.tan));
	float3 B = normalize(mul(normalMat, Input.bitan));
	float3 N = normalize(mul(normalMat, Input.normal));

	//T = normalize(T - dot(T, N) * N);

	float3x3 mat = { T,B,N };
	vso.TBN = mat;

	//TBN = transpose(TBN);

	float3 FragPos = (float3)mul(u_Transform, totalPosition);

	vso.fragPos = FragPos;
	vso.viewPosition = u_viewPosition;
	return vso;
}
#type pixel

struct Material
{
	float3 albedo;
	float shininess;
	float metallic;
	float roughness;
	float ambientocclusion;

	bool IsAlbedoMap;
	bool IsNormalMap;
	bool IsMetallicMap;
	bool IsRoughnessMap;
	bool IsAoMap;
};
struct PointLight
{
	float3 position;
	float3 diffuse;
	float3 ambient;
	float3 specular;

	float constant;
	float Linear;
	float quadratic;
};

cbuffer material : register(b2)
{
	Material material;
}
cbuffer light : register(b3)
{
	PointLight pointLight[1];
}
cbuffer light : register(b4)
{
	bool gamma;
	bool flip;
}
Texture2D diffuseTex;
Texture2D normalMapTex;
Texture2D specularTex;
Texture2D ambientTex;

SamplerState splr : register(s0);
struct PSIn
{
	float2 tc			     : Texcoord;
	float3 normal		     : Normal;
	float3 fragPos			 : FragPos;
	float3x3 TBN		     : TBN;
	float3 viewPosition      : ViewPos;
	float4 pos :SV_Position;
};
struct PS_OUT
{
	float4 color :SV_TARGET0;
};
// calculates the color when using a point light.
float3 CalcPointLight(PointLight light, float3 normal, float3 fragPos,float3 viewDir, float2 tc, Texture2D diffuseTex, Texture2D specularTex,Texture2D ambientTex)
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
	float3 ambient = light.ambient * ambientTex.Sample(splr, tc).rgb;
	float3 diffuse = light.diffuse * diff * diffuseTex.Sample(splr, tc).rgb;
	float3 specular = light.specular * spec * specularTex.Sample(splr, tc).rgb;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}
PS_OUT PSMain(PSIn input)
{
	float4 texcolor = diffuseTex.Sample(splr, input.tc).rgba;
	clip(texcolor.a < 0.1f ? -1 : 1);

	PS_OUT output;
	float4 color;
	float3 norm;
	float3 viewDir = normalize(input.viewPosition - input.fragPos);
	if (material.IsNormalMap)
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

	// dirLight
	//result += CalcDirLight(dirLight, norm,viewDir,input.tc,input.fragPosLightSpace,diffuseTex, specularTex);
	// point light
	result += CalcPointLight(pointLight[0], norm, input.fragPos, viewDir, input.tc, diffuseTex, specularTex, ambientTex);
	if (gamma)
		result = pow(result,1.0f / 2.2f);

	output.color = float4(result, texcolor.a);
	return output;
}