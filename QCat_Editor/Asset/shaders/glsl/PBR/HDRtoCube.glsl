// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 localPos;


void main()
{
	localPos = a_Position;
	gl_Position = u_Projection * u_View * vec4(localPos , 1.0f);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 localPos;

uniform sampler2D equirectangularMap;
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
	vec2 uv = SampleSphericalMap(normalize(localPos));
	vec3 result = texture(equirectangularMap,uv).rgb;
	color = vec4(result,1.0f);
}
