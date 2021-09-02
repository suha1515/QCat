#type compute
#version 450 core
struct Data
{
	vec3 a;
	vec2 b;
};
layout(std430,binding =0)  buffer Data1
{
	Data input1[];
};

layout(std430,binding =1)  buffer Data2
{
	Data input2[];
};

layout(std430,binding =2)  buffer Data3
{
	Data outputData[];
};

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main()
{
  uint index = gl_GlobalInvocationID.x;

  outputData[index].a = input1[index].a + input2[index].a;
  outputData[index].b = input1[index].b + input2[index].b;
}