#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec3 a_Color;

out VS_OUT {
	vec3 color;
} vs_out;

void main()
{

	vec4 pos =  vec4(a_Position.x,a_Position.y,0.0,1.0);
	vs_out.color =a_Color;
	gl_Position = pos;
}
#type geometry
#version 330 core
layout(points) in;
layout(triangle_strip,max_vertices =5) out;

in VS_OUT {
	vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{
	fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex(point)
	gl_Position = position + vec4( 0.2,-0.2,0.0,0.0);
	EmitVertex();

	gl_Position = position + vec4(-0.2,-0.2,0.0,0.0);
	EmitVertex();

	gl_Position = position + vec4( 0.2, 0.2,0.0,0.0);
	EmitVertex();

	gl_Position = position + vec4(-0.2, 0.2,0.0,0.0);
	EmitVertex();

	fColor = vec3(1.0,1.0,1.0);
	gl_Position = position + vec4( 0.0, 0.4,0.0,0.0);
	EmitVertex();
	EndPrimitive();
}

void main(){
	build_house(gl_in[0].gl_Position);
	//gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.2,0.0,0.0);
	//EmitVertex();
	//gl_Position = gl_in[0].gl_Position + vec4(0.0,-0.2,0.0,0.0);
	//EmitVertex();
	//EndPrimitive();
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 fColor;
void main()
{
	color = vec4(fColor,1.0f);
}

