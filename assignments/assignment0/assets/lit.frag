#version 450

in Surface{
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}fs_in;

out vec4 FragColor;

uniform sampler2D _MainTex; //2D texture samp
uniform vec3 _EyePos;
//lighting downward
uniform vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
uniform vec3 _LightColor = vec3(1.0); //white
uniform vec3 _AmbientColor = vec3(0.3, 0.4, 0.46);
void main()
{
	//fragnormal still length 1 after interpol
	vec3 normal = normalize(fs_in.WorldNormal);

	//lighting down
	vec3 toLight = -_LightDirection;

	float diffuseFactor = max(dot(normal, toLight), 0.0);

	//direction to eye
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);

	//Blinn-Phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), 128);

	//combination of specular and diffuse reflection
	vec3 lightColor = (diffuseFactor + specularFactor) * _LightColor;
	//Add ambient
	lightColor += _AmbientColor;
	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	
	FragColor = vec4(objectColor * lightColor, 1.0);
}