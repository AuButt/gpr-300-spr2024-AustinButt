#version 450

out vec4 FragColor;

in vec3 toCamera;
in vec2 vs_TexCoords;

uniform sampler2D waterSpec;
uniform sampler2D waterTexture;
uniform sampler2D waterWarp;

uniform float texScale;
uniform float warpScale;
uniform float specScale;
uniform float warpStrength;

uniform vec3 water_color;
uniform sampler2D tex0;
uniform float time;


uniform float tiling;


void main()
{
	vec2 uv = vs_TexCoords;

	//warp
	vec2 warp_uv = vs_TexCoords * warpScale;
	vec2 warp_scroll = vec2(0.5, 0.5) * time;
	vec4 warp = texture(waterWarp, warp_uv + warp_scroll).xy * warpStrength;

	warp = (warp * 2.0 - 1.0);

	//albedo 
	vec2 albedo_uv = vs_TexCoords * tiling;
	vec2 albedo_scroll = vec2(-0.5, 0.5) * time;
	vec4 albedo = texture(waterWarp, albedo_uv + warp + albedo_scroll);

	//specular
	vec2 spec_uv = vs_TexCoords * specScale;
	vec3 smp1 = texture(water_spec, spec_uv + vec2(1.0, 0.0) * time).rgb;
	vec3 smp2 = texture(water_spec, spec_uv + vec2(1.0, 0.0) * time).rgb;
	vec3 spec = smp1 + smp2;

	float brightness = dot(spec, vec3(0.299, 0.587, 0.114));
	if(brightness <= brightness_lower_cutoff || brightness >= brightness_upper_cutoff)
	{
		//too bright/dark

	}
	else{
		
	}

	FragColor = vec4(albedo, 1.0);
}
