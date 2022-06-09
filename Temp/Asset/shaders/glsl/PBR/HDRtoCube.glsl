// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140,binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
};
struct VertexOutput
{
	vec3 localPos;
};

layout(location = 0) out VertexOutput Output;

void main()
{
	Output.localPos = a_Position;
	gl_Position = u_Projection * u_View * vec4(a_Position , 1.0f);
}

#type fragment
#version 450 core
layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec3 localPos;
};

layout(location = 0) in VertexOutput Input;

layout (binding = 0) uniform sampler2D equirectangularMap;
const vec2 invAtan = vec2(0.1591,0.3183);

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.x,v.z),asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(Input.localPos));
	vec3 result = texture(equirectangularMap,uv).rgb;
	color = vec4(result,1.0f);
}
