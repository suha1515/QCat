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

uniform samplerCube envMap;

const float PI = 3.14159265359;

void main()
{
	// world vector cacts as the normal of tangent space
	// fromt the origin, aligned to worldpos, Given this normal, calclulate all
	// incoming radiance of the evnrioenment . the result of this radiance
	// is the radiance of light coming from - normal direction, which is what
	// we use in the pbr shader to sample irradiance
	vec3 N = normalize(localPos);
	vec3 irradiance = vec3(0.0);

	// tangent space calculation from orign point
	vec3 up = vec3(0.0f,1.0f,0.0f);
	vec3 right =  normalize(cross(up,N));
	up = normalize(cross(N,right));

	 float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += texture(envMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    color = vec4(irradiance,1.0);
}
