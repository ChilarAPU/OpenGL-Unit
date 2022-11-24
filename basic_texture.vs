#version 460

uniform mat4 mvpMatrix;
//holds the model matrix for moving normals
uniform mat4 model;

layout (location=0) in vec4 vertexPos;
layout (location=3) in vec2 vertexTexCoord;
layout (location=6) in vec3 vertexNormal;

out vec2 texCoord;
out vec3 pixelNormal;
out vec3 crntPos; //vertex position

vec3 L = vec3(2.0, 2.0, 0.0); //lights position
vec3 Lk = vec3(1.0, 0.5, 0.0); //attenuation properties
vec3 Lcolour = vec3(0.6, 0.6, 0.6); //light colour


void main(void) {


	mat4 M;
	M[0] = vec4(1.0);

	ivec2 a = ivec2(1, 2);
	//vec3 b = vec3(2.0, 4.0, 1.0) + a;

	pixelNormal = mat3(transpose(inverse(model))) * vertexNormal;
	texCoord = vertexTexCoord;

	//translate position into a vector3
	vec3 pos = vec3(vertexPos.x, vertexPos.y, vertexPos.z);
	
	gl_Position = mvpMatrix * vertexPos;
}
