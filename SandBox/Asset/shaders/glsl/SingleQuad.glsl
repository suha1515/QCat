// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoords;

void main()
{
	TexCoords = a_TexCoord;
	gl_Position = vec4(a_Position.x,a_Position.y,0.0,1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D screenTexture;
const float offsetX = 1.0 / 1600.0;  
const float offsetY = 1.0 / 900.0;
void main()
{
	//vec2 offsets[9] = vec2[](
    //    vec2(-offsetX,  offsetY), // top-left
    //    vec2( 0.0f,    offsetY), // top-center
    //    vec2( offsetX,  offsetY), // top-right
    //    vec2(-offsetX,  0.0f),   // center-left
    //    vec2( 0.0f,    0.0f),   // center-center
    //    vec2( offsetX,  0.0f),   // center-right
    //    vec2(-offsetX, -offsetY), // bottom-left
    //    vec2( 0.0f,   -offsetY), // bottom-center
    //    vec2( offsetX, -offsetY)  // bottom-right    
    //);

	// sharpen
   //float kernel[9] = float[](
   //    -1, -1, -1,
   //    -1,  9, -1,
   //    -1, -1, -1
   // );
   // blur
	//float kernel[9] = float[](
   // 1.0 / 16, 2.0 / 16, 1.0 / 16,
   // 2.0 / 16, 4.0 / 16, 2.0 / 16,
   // 1.0 / 16, 2.0 / 16, 1.0 / 16  
	//);
	// edge detection
	float kernel[9] = float[](
	    1,  1, 1,
	    1, -8, 1,
	    1, 1,  1
	 );
	vec4 FragColor = texture(screenTexture,TexCoords);
	//Inversion
	//color = vec4(vec3(1.0 - texture(screenTexture,TexCoords)),1.0);
	// GrayScale
	//float average =  0.2126 * FragColor.r +0.7152 * FragColor.g +  0.0722 * FragColor.b;
    //color = vec4(average, average, average, 1.0);
	// Kernels Sharpen
	//vec3 col = vec3(0.0);
    //for(int i = 0; i < 9; i++)
    //{
    //    col += vec3(texture(screenTexture, TexCoords.st + offsets[i]))* kernel[i];
    //}
    //color = vec4(col,1.0);
	color = texture(screenTexture,TexCoords);
}