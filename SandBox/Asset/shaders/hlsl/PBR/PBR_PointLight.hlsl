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
struct PointLight
{
	float3 position;
	float3 diffuse;
};
struct Material
{
	bool IsNormalMap;
};
cbuffer light : register(b0)
{
	PointLight pointLight[4];
}
cbuffer material : register(b1)
{
	Material material;
}
Texture2D albedoMap;
Texture2D normalMap;
Texture2D metallicMap;
Texture2D roughnessMap;
Texture2D aoMap;

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

PS_OUT PSMain(PSIn input)
{
	PS_OUT output;

	float3 albedo = pow(albedoMap.Sample(splr, input.tc).rgb, 2.2f);
	float metallic = metallicMap.Sample(splr, input.tc).r;
	float roughness = roughnessMap.Sample(splr, input.tc).r;
	float ao = aoMap.Sample(splr, input.tc).r;

	float4 color;
	float3 N;
	float3 V = normalize(input.viewPosition - input.fragPos);
	if (material.IsNormalMap)
	{
		N = normalMap.Sample(splr, input.tc).rgb;
		N = N * 2.0f - 1.0f;
		N = normalize(mul(input.TBN, N));
	}
	else
	{
		N = normalize(input.normal);
	}

	float3 result = float3(0.0f, 0.0f, 0.0f);
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		float3 L = normalize(pointLight[i].position - input.fragPos);
		float3 H = normalize(V + L);
		float distance = length(pointLight[i].position - input.fragPos);
		float attenuation = 1.0 / (distance * distance);
		float3 radiance = pointLight[i].diffuse * attenuation;

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
	}

	float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;
	result = ambient + Lo;
	//HDR tonemapping
	result = result / (result + float3(1.0f, 1.0f, 1.0f));
	result = pow(result, 1.0f / 2.2f);
	
	output.color = float4(result, 1.0f);
	return output;
}