#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;
out vec4 lightPosEye;
out vec4 lightPosEye2;
out vec4 lightPosEye3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;
uniform int fog;
uniform int sandstorm;
void main() 
{

	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);
	lightPosEye = view* model * vec4(19.908f, 0.31856f, -6.171f,1);
	lightPosEye2 = view* model * vec4(6, 15, 15,1);
	lightPosEye3 = view* model * vec4(5.929f, 2.5237f, 15.161f,1);

	fPosition = vPosition;
	fNormal = vNormal;
	fTexCoords = vTexCoords;	
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);

}
