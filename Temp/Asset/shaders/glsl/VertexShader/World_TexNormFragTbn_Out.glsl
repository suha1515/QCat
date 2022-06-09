#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_BitTangent;


uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec3 viewPosition;

out vec2 TexCoords;
out vec3 v_Normal;
out vec3 FragPos;
out mat3 TBN;

void main()
{
	TexCoords = a_TexCoord;
	mat3 normalMatrix = mat3(transpose(inverse(u_Transform)));
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	gl_Position = u_ViewProjection *u_Transform * vec4(a_Position, 1.0);
	FragPos = vec3(u_Transform * vec4(a_Position,1.0));

	vec3 T = normalize(normalMatrix *a_Tangent);
	vec3 B = normalize(normalMatrix *a_BitTangent);
	vec3 N = normalize(normalMatrix *a_Normal);
	// re-orthogonalize T With to N
	//T = normalize(T - dot(T,N) * N);
	//vec3 B = cross(N,T);
	TBN = mat3(T,B,N);

}