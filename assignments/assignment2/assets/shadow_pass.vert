#version 450

//Vertex attributes
layout (location = 0) in vec3 vPos; //vertex pos

out vec3 vs_normal;

uniform mat4 transformModel; 
uniform mat4 light_view_proj; 

void main()
{
	vec4 world_position = transformModel * vec4(vPos, 1.0);
	//light view
	gl_Position = light_view_proj * world_position;
}