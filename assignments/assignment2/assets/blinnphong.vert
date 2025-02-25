#version 450

//Vertex attributes
layout (location = 0) in vec3 vPos; //vertex pos
layout (location = 1) in vec3 vNorm; //vertex norm
layout (location = 2) in vec2 vTexCoord; //Vertex texture coord (UV)

uniform mat4 transformModel; //model -> world matrix
uniform mat4 viewProjection; //cam viewProjection
uniform mat4 light_view_proj;

out vec4 frag_pos_lightspace;
out vec3 vs_frag_light_position;
out vec3 vs_normal;
out vec2 vs_texcoord;
out vec3 vs_position;

void main()
{
	vec4 world_position = transformModel * vec4(vPos, 1.0);
	vs_position = vec3(transformModel * vec4(vPos, 1.0));

	vs_normal = transpose(inverse(mat3(transformModel))) * vNorm;	
	vs_texcoord = vTexCoord;
	
	frag_pos_lightspace = world_position * light_view_proj;

	gl_Position = viewProjection * world_position;
}