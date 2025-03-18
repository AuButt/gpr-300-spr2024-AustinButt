#version 450

//Vertex attributes
layout (location = 0) in vec3 vPos; //vertex pos
layout (location = 1) in vec3 vNorm; //vertex norm
layout (location = 2) in vec2 vTextCoord; //Vertex texture coord (UV)

out Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}vs_out;

uniform mat4 transformModel; //model -> world matrix
uniform mat4 viewProjection; // combined view -> proj matrix

void main()
{
	//Transform vertex pos to World Space
	vs_out.WorldPos = vec3(transformModel * vec4(vPos, 1.0));
	//Transform vertex normal to world space using Normal Matrix
	vs_out.WorldNormal = transpose(inverse(mat3(transformModel))) * vNorm;
	vs_out.TexCoord = vTextCoord;

	//Transform vertex pos to clip space
	gl_Position = viewProjection * transformModel * vec4(vPos, 1.0);
}