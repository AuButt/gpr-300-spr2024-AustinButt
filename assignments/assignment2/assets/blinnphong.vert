#version 450

//Vertex attributes
layout (location = 0) in vec3 vPos; //vertex pos
layout (location = 1) in vec3 vNorm; //vertex norm
layout (location = 2) in vec2 vTexCoord; //Vertex texture coord (UV)

uniform mat4 transformModel; //model -> world matrix
uniform mat4 viewProjection; //cam viewProjection
uniform mat3 light_view_proj;

out vec3 vs_frag_world_position;
out vec3 vs_frag_light_position;
out vec3 vs_normal;
out vec2 vs_texcoord;

void main()
{
	vec4 world_position = transformModel * vec4(vPos, 1.0);

	vs_frag_world_position = world_position.xyz;
	vs_frag_light_position = light_view_proj * world_position;
	vs_normal = transpose(inverse(mat3(transformModel))) * vNorm;
	vs_texcoord = vTexCoord;

	gl_Position = viewProjection * world_position;
}