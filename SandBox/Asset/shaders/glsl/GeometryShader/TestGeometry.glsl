#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;

void main()
{

	vec4 pos =  vec4(a_Position.x,a_Position.y,0.0,1.0);
	gl_Position = pos;
}
#type geometry
#version 330 core
layout(points) in;
layout(points,max_vertices =1) out;

void main(){
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

void main()
{
	color = vec4(0.0f,1.0f,0.0f,1.0f);
}

