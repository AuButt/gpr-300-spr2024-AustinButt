#version 450

out vec4 FragColor;

in vec3 toCamera;
in vec2 vs_TexCoords;

uniform vec3 water_color;
uniform sampler2D tex0;
uniform float time;

uniform float b1;
uniform float b2;

uniform float tiling;

const vec3 reflectColor = vec3(1.0f, 0.0f, 0.0f);

void main()
{
	vec2 dir = normalize(vec2(1.0));
	vec2 uv = vs_TexCoords * tiling;

    uv.y += 0.01 * (sin(uv.x * 3.5 + time * 0.35) + sin(uv.x * 4.8 + time * 1.05) + sin(uv.x * 7.3 + time * 0.45)) / 3.0;
    uv.x += 0.12 * (sin(uv.y * 4.0 + time * 0.5) + sin(uv.y * 6.8 + time * 0.75) + sin(uv.y * 11.3 + time * 0.2)) / 3.0;
    uv.y += 0.12 * (sin(uv.x * 4.2 + time * 0.64) + sin(uv.x * 6.3 + time * 1.65) + sin(uv.x * 8.2 + time * 0.45)) / 3.0;

	float fresnelFactor = dot(normalize(toCamera), vec3(0.0, 1.0, 0.0));

	vec3 albedo = texture(tex0, uv + (time * dir)).rgb;

	vec3 color = mix(reflectColor, water_color, fresnelFactor);
	color = mix(color, albedo, 0.5f);


	vec4 smp1 = texture(tex0, uv + time * 0.2);
	vec4 smp2 = texture(tex0,uv + vec2(0.2) + time * 0.2);

	vec3 colorr = water_color + vec3(smp1.r * b1 - smp2.r * b2);

	FragColor = vec4(colorr, 1.0);
}
