// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_BitTangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;
out vec3 Normal;

uniform mat4 view;
uniform mat4 invView;
uniform mat4 proj;
uniform mat4 u_Transform;
uniform mat4 u_invTransform;

uniform bool flip;

void main()
{
	vec4 viewPos = view * u_Transform * vec4(a_Position,1.0f);
	FragPos = viewPos.xyz;
	TexCoords = a_TexCoord;


	// (AB)^-1 = B^-1A^-1
	mat3 normalMatrix = transpose( mat3( u_invTransform*invView));
	Normal = normalMatrix * (flip ? -a_Normal : a_Normal);

	gl_Position = proj  * viewPos;

	vec3 T = normalize(normalMatrix *a_Tangent);
	vec3 B = normalize(normalMatrix *a_BitTangent);
	vec3 N = normalize(normalMatrix *a_Normal);
	// re-orthogonalize T With to N
	//T = normalize(T - dot(T,N) * N);
	//vec3 B = cross(N,T);
	TBN = mat3(T,B,N);

}

#type fragment
#version 330 core
struct Material
{
	vec3 diffuse;
	bool normalMap;
};

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;
in vec3 Normal;

uniform Material material;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

void main()
{
	
	vec3 norm;
	gTangent = T;
	if(material.normalMap)
	{
		norm = texture(texture_normal,TexCoords).rgb;
		norm.r = norm.r * 2.0 - 1.0;
		norm.g = norm.g * 2.0 - 1.0;
		norm.b = norm.b * 2.0 - 1.0;
		norm = normalize(TBN*norm);
	}
	else
	{
		norm = normalize(Normal);
	}
	gNormal = norm;
	gPosition = FragPos;
	gAlbedoSpec.rgb = texture(texture_diffuse,TexCoords).rgb * material.diffuse;
	gAlbedoSpec.a = texture(texture_specular,TexCoords).r;
}