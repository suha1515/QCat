#type vertex

cbuffer trnasform  : register(b0)
{
	matrix u_ViewProjection;
	matrix u_Transform;
	matrix u_invTransform;
	float3 u_viewPosition;
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
};
VSOut VSMain(VSIn Input)
{
	VSOut vso;

	matrix viewprojMat = mul(u_ViewProjection, u_Transform);
	float3x3 normalMat = (float3x3)transpose(u_invTransform);
	vso.pos = mul(viewprojMat, float4(Input.pos, 1.0f));
	vso.tc = Input.tc;
	vso.normal = mul(normalMat, Input.normal);

	float3 T = normalize(mul(normalMat, Input.tan));
	float3 B = normalize(mul(normalMat, Input.bitan));
	float3 N = normalize(mul(normalMat, Input.normal));

	//T = normalize(T - dot(T, N) * N);

	float3x3 mat = { T,B,N };
	vso.TBN = mat;

	//TBN = transpose(TBN);

	float3 FragPos = (float3)mul(u_Transform, float4(Input.pos, 1.0f));
	vso.fragPos = FragPos;
	vso.viewPosition = u_viewPosition;
	return vso;
}
#type pixel

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
}
Texture2D diffuseTex;
Texture2D specularTex;
Texture2D normalMapTex;
Texture2D shadowMapTex : register(t4);

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

PS_OUT PSMain(PSIn input)
{
	float4 texcolor = diffuseTex.Sample(splr, input.tc).rgba;
	clip(texcolor.a < 0.1f ? -1 : 1);

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

	// dirLight
	//result += CalcDirLight(dirLight, norm,viewDir,input.tc,input.fragPosLightSpace,diffuseTex, specularTex);
	// point light
	result += CalcPointLight(pointLight, norm, input.fragPos, viewDir, input.tc, diffuseTex, specularTex);
	if (gamma)
		result = pow(result,1.0f / 2.2f);

	output.color = float4(result, texcolor.a);
	return output;
}