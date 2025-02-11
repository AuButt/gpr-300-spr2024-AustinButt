#version 450

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Norm;
layout(location = 2) in vec2 in_TexCoord;

uniform vec3 cameraPos;
uniform mat4 transformModel; //model -> world matrix
uniform mat4 viewProjection; // combined view -> proj matrix
uniform float time;

out vec2 vs_TexCoords;
out vec3 toCamera;


void main()
{
	vec4 world_position = transformModel * vec4(in_Pos, 1.0f);
	toCamera = cameraPos - world_position.xyz;
	
	vs_TexCoords = in_TexCoord;

	gl_Position = viewProjection * transformModel * vec4(in_Pos, 1.0f);
}