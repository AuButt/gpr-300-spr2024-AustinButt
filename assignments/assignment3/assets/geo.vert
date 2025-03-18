#version 450

//Vertex attributes
layout (location = 0) in vec3 vPos; //vertex pos
layout (location = 2) in vec2 vTextCoord; //Vertex texture coord (UV)

out vec2 vs_texCoord;

void main()
{
	vs_texCoord = vTextCoord;
	gl_Position  = vec4(vPos, 1.0);
}