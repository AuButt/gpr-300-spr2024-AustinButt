#version 450

out vec4 FragColor;

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light{
	vec3 color;
	vec3 position;
};

struct Ambient{
	vec3 color;
	float intensity;
};

// ins
in vec3 vs_position;
in vec3 vs_normal;
in vec3 vs_texcoord;

//uniforms
uniform Material material;
uniform vec3 camera_position;
uniform Light light;
uniform sampler2D shadowMap;

float ShadowCalculations(vec4 frag_pos_lightspace)
{
	vec3 projCoord = frag_pos_lightspace.xyz / frag_pos_lightspace.xyz;
	projCoord = (projCoord * 0.5) + 0.5;

	float closestDepth = texture(shadowMap, projCoord.xy).r;
	float currDepth = projCoord.z;

	float shadow = (currDepth > closestDepth) ? 1.0 : 0.0;

	return shadow;
}

vec3 blinnphong(vec3 normal, vec3 frag_pos)
{
	//normalize inputs
	vec3 view_dir = normalize(camera_position - frag_pos);
	vec3 light_dir = normalize(light.position - frag_pos);
	vec3 halfway_dir = normalize(light_dir + view_dir);

	//dot products
	float ndotl = max(dot(normal, light_dir), 0.0);
	float ndoth = max(dot(normal, halfway_dir), 0.0);

	//light components
	vec3 diffuse = ndotl * material.diffuse;
	vec3 specular = pow(ndoth, material.shininess * 128.0) * material.specular;

	return (diffuse + specular);
}

void main()
{
	vec3 normal = normalize(vs_normal);

	float shadow = ShadowCalculations();

	vec3 lighting = blinnphong(normal, vs_position);
	lighting *= light.color;
	lighting *= vec3(1.0) * material.ambient;

	vec3 object_color = normal * 0.5 + 0.5;

	FragColor = vec4(object_color * lighting, 1.0);
}