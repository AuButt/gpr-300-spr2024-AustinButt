#version 450

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Norm;
layout(location = 2) in vec2 in_TexCoord;

uniform vec3 cameraPos;
uniform mat4 transformModel; //model -> world matrix
uniform mat4 viewProjection; // combined view -> proj matrix
uniform float time;

out vec2 vs_TexCoords;
out vec3 toCamera;

float calculateSurface(float x, float z) {
  float scale = 10.0;
  float y = 0.0;
  y += (sin(x * 1.0 / scale + time * 1.0) + sin(x * 2.3 / scale + time * 1.5) + sin(x * 3.3 / scale + time * 0.4)) / 3.0;
  y += (sin(z * 0.2 / scale + time * 1.8) + sin(z * 1.8 / scale + time * 1.8) + sin(z * 2.8 / scale + time * 0.8)) / 3.0;
  return y;
}

void main()
{
	vec3 pos = in_Pos;
	pos += calculateSurface(pos.x, pos.y);

	vec4 world_position = transformModel * vec4(in_Pos, 1.0f);
	toCamera = cameraPos - world_position.xyz;
	
	vs_TexCoords = in_TexCoord;

	gl_Position = viewProjection * transformModel * vec4(pos, 1.0f);
}