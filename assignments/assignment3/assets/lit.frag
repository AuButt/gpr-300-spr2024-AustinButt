#version 450

in Surface{
	vec3 WorldPos; //vertex pos in world space
	vec3 WorldNormal; //vertex normal
	vec2 TexCoord;
}fs_in;

struct Material
{
	float Ka; //ambient coeff 0-1
	float Kd; //diffuse coeff 0-1
	float Ks; //ambient coeff 0-1
	float Shininess; //size of specular highlight
	vec3 rgb_normal ;
};
uniform Material _Material;

uniform sampler2D _MainTex; //2D texture samp
uniform sampler2D _NormalMap; //stones map
uniform vec3 _LightColor = vec3(1.0); //white
uniform vec3 cameraPos;
uniform vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
uniform vec3 _AmbientColor = vec3(0.3, 0.4, 0.46);

out vec4 FragColor; //color of fragment

void main()
{
	//fragnormal still length 1 after interpol
	vec3 normal = normalize(fs_in.WorldNormal);

	// obtain normal from normal map in range [0,1]
    normal = texture(_NormalMap, fs_in.TexCoord).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  
	
	//lighting down
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight), 0.0);
	
	//direction to eye
	vec3 toEye = normalize(cameraPos - fs_in.WorldPos);

	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);

	//combination of specular and diffuse reflection
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	//Add ambient
	lightColor += _AmbientColor * _Material.Ka;

	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);
}