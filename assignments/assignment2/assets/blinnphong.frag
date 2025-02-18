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

float shadow_calculation(vec4 frag_pos_lightspace)
{
	
	

	float shadow = (camera_depth > lightdepth) ? 1.0:0.0;
	
	return 0.0f;
}

vec3 blinnphong(vec3 normal, vec3 frag_pos)
{
	//normalize inputs
	vec3 view_dir = normalize(camera_position - frag_pos);
	vec3 light_dir = normalize(light.position - frag_pos);
	vec3 halfway_dir = normalize(light_dir + view_dir);

	//dot products
	float ndotl = max(dot(normal, light.color), 0.0);
	float ndoth = max(dot(normal, halfway_dir), 0.0);

	//light components
	vec3 diffuse = ndotl * vec3(material.diffuse);
	vec3 specular = pow(ndoth, material.shininess * 128.0) * material.specular;

	return (diffuse + specular);
}

void main()
{
	vec3 normal = normalize(vs_normal);

	float shadow = shadow_calculation(vec4(0.0));

	vec3 lighting = blinnphong(normal, vs_position);

	lighting *= light.color;
	lighting *= vec3(1.0) * material.ambient;

	vec3 object_color = normal * 0.5 + 0.5;

	FragColor = vec4(object_color * lighting, 1.0);
}