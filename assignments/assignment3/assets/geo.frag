#version 450

out vec4 FragColor;

in vec2 vs_texCoord;

uniform sampler2D g_albedo;
uniform sampler2D g_position;
uniform sampler2D g_normal;

void main()
{
	vec3 color = texture(g_position, vs_texCoord).rgb;
	FragColor = vec4(color, 1.0);
}