#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;
out vec4 lightPosEye;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;

void main() 
{

	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);
	lightPosEye = view* model * vec4(5.929f, 2.5237f, 18.161f,1);

	fPosition = vPosition;
	fNormal = vNormal;
	fTexCoords = vTexCoords;	
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);

}
