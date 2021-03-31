// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat4 u_invTransform;

void main()
{
	vec4 worldPos = u_Transform * vec4(a_Position,1.0f);
	FragPos = worldPos.xyz;
	TexCoords = a_TexCoord;

	mat3 normalMatrix = transpose(mat3(u_invTransform));
	Normal = normalMatrix * a_Normal;

	gl_Position = u_ViewProjection  * worldPos;
}

#type fragment
#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{
	gPosition = FragPos;
	gNormal = Normal;
	gAlbedoSpec.rgb = texture(texture_diffuse,TexCoords).rgb;
	gAlbedoSpec.a = texture(texture_specular,TexCoords).r;
}