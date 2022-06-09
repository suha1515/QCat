// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoords;

void main()
{
	TexCoords = a_TexCoord;
	gl_Position = vec4(a_Position,1.0);
}

#type fragment
#version 330 core
layout(location = 0) out float color;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[128];
uniform vec2 screenSize;
uniform mat4 Projection;

int kernelSize = 128;
float radius = 0.5f;
float bias = 0.025f;


void main()
{
    vec2 noiseScale = vec2(screenSize.x/4.0f,screenSize.y/4.0f);

	vec3 fragPos = texture(gPosition,TexCoords).rgb;
	vec3 normal  = normalize(texture(gNormal,TexCoords).rgb);
	vec3 randomVec = normalize(texture(texNoise,TexCoords*noiseScale ).xyz);

	// Create TBN tan space to world
	vec3 tangent = normalize(randomVec - normal * dot(randomVec,normal));
	vec3 bitangent = cross(normal,tangent);

	mat3 TBN = mat3(tangent,bitangent,normal);
	float occlusion = 0.0;
	for(int i=0;i<kernelSize;++i)
	{
		vec3 samplePos = TBN * samples[i]; // tan to view
		samplePos = fragPos + samplePos * radius;

		vec4 offset = vec4(samplePos,1.0);
		offset = Projection * offset;  // from world to clip
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0~1.0

		float sampleDepth = texture(gPosition,offset.xy).z; // get depth value of kernel sample

		//range check & accumulate
		float rangeCheck = smoothstep(0.0,1.0,radius/abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth <= samplePos.z + bias ? 1.0 : 0.0) *rangeCheck ;
	}
	occlusion = 1.0 - (occlusion / kernelSize);

	color  = occlusion;
}
