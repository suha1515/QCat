// Flat Color Shader
#type vertex
#version 450 core
struct VertexOutput
{
	vec2 q;
	vec2 b1;
	vec2 b2;
	vec2 b3;
};

layout(std140,binding = 0) uniform Camera
{
	mat4 u_Projection;
	mat4 u_View;
	vec3 viewPosition;
};
layout(std140,binding = 1) uniform Corners
{
	vec4 corners[4];
};
layout(location = 0 ) out VertexOutput Output;

void main()
{
	vec4 pos = corners[gl_VertexIndex];
	// Set Up Inverse bilinear interpolation
	Output.q =  vec2(pos - corners[0]);
	Output.b1 =	vec2(corners[2] - corners[0]); 
	Output.b2 =  vec2(corners[1] - corners[0]);
	Output.b3 =  vec2(corners[0] - corners[1] - corners[2] + corners[3]);
	gl_Position = u_Projection * u_View * pos;
}

#type fragment
#version 450 core
struct VertexInput
{
	vec2 q;
	vec2 b1;
	vec2 b2;
	vec2 b3;
};
layout(location = 0 ) in VertexInput Input;
layout(location = 0) out vec4 color;
layout (binding =0 ) uniform sampler2D image;

float Wedge2D(vec2 v,vec2 w)
{
	return v.x * w.y - v.y*w.x;
}
void main()
{
	//Set up Quadratic Formula
	float A = Wedge2D(Input.b2,Input.b3);
	float B = Wedge2D(Input.b3,Input.q) - Wedge2D(Input.b1,Input.b2);
	float C = Wedge2D(Input.b1,Input.q);

	//Solve for V
	vec2 uv;
	if(abs(A)<0.001)
	{
		uv.y = -C/B;
	}
	else
	{
		// Quadratic form , Take Positive root for CCW winding with V-Up
		float discrim = B*B - 4*A*C;
		uv.y = 0.5* (-B +sqrt(discrim))/A;
	}
	// solve for u, using largets-magnitude component
	vec2 denom = Input.b1 + uv.y * Input.b3;
	if(abs(denom.x)>abs(denom.y))
		uv.x = (Input.q.x - Input.b2.x * uv.y) / denom.x;
	else
		uv.x = (Input.q.y - Input.b2.y * uv.y) / denom.y;

	vec4 col = texture(image,uv);
	if(col.a < 0.1)
		discard;
	color = col;
}