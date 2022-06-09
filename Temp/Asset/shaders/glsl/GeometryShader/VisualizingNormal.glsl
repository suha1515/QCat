#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

out VS_OUT {
	vec3 normal;
} vs_out;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat4 u_invTransform;

void main()
{
	mat3 normalMatrix = mat3(transpose(u_ViewProjection * u_invTransform));
	vs_out.normal = normalize(normalMatrix * a_Normal);
	gl_Position = u_ViewProjection *u_Transform * vec4(a_Position,1.0f);
}
#type geometry
#version 330 core
layout(triangles) in;
layout(line_strip,max_vertices =6) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;

void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal,0.0) * MAGNITUDE;
	EmitVertex();

	EndPrimitive();
}

void main(){
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0f,1.0f,0.0f,1.0f);
}


