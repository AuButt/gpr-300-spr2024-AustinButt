#version 450

//Vertex attributes
layout (location = 0) in vec2 in_position; //vertex pos
layout (location = 1) in vec2 in_texcoord; //Vertex texture coord (UV)

out vec2 vs_texcoord;

void main()
{
	vs_texcoord = in_texcoord;
	gl_Position = vec4(in_position.xy, 0.0, 1.0);
}