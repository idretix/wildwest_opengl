#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 lightPosEye;
in vec4 lightPosEye2;
in vec4 lightPosEye3;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
//lighting2
uniform vec3 lightDir2;
uniform vec3 lightColor2;
//portal
uniform vec3 lightDir3;
uniform vec3 lightColor3;
//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

vec3 diffuse2;
vec3 specular2;

vec3 diffuse3;
vec3 specular3;

uniform int fog;
uniform int sandstorm;
uniform int transparent;

in vec4 fragPosLightSpace;
uniform sampler2D shadowMap;

float computeShadow()
{
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    

    float currentDepth = normalizedCoords.z;
    float bias = 0.005f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}

float computeFog(){

    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    float fogDensity=0.05f;
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance*fogDensity,2));

    return clamp(fogFactor,0.0f,1.0f);
}

float computeSandstorm(){
    float fogDensity=0.00f;
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    for(int i=1;i<=100;i++){
        fogDensity+=0.005;
    }
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance*fogDensity,2));

    return clamp(fogFactor,0.0f,1.0f);
}

void computeDirLight()
{
   	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin

    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction

    vec3 lightDirN = vec3(normalize(lightPosEye.xyz - fPosEye.xyz));

    
float constant = 1.0f;
float linear = 0.7f; float quadratic = 1.8f;
float dist = length(lightPosEye.xyz - fPosEye.xyz);
float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
////

    vec3 lightDirN2 = vec3(normalize(lightPosEye2.xyz - fPosEye.xyz));
float constant2 = 1.0f;
float linear2 = 0.00014f; float quadratic2 = 0.000007f;
float dist2 = length(lightPosEye2.xyz - fPosEye.xyz);
float att2 = 1.0f / (constant2 + linear2 * dist2 + quadratic2 * (dist2 * dist2));
////////////
 vec3 lightDirN3 = vec3(normalize(lightPosEye2.xyz - fPosEye.xyz));
float constant3 = 1.0f;
float linear3 = 0.22f; float quadratic3 = 0.20f;
float dist3 = length(lightPosEye3.xyz - fPosEye.xyz);
float att3 = 1.0f / (constant3 + linear3 * dist3 + quadratic3 * (dist3 * dist3));
    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = att*ambientStrength * lightColor;
   // ambient2 = att2*ambientStrength * lightColor;

    //compute diffuse light
    diffuse = att*max(dot(normalEye, lightDirN), 0.0f) * lightColor2;
    diffuse2 = att2*max(dot(normalEye, lightDirN2), 0.0f) * lightColor;
    diffuse3 = att3*max(dot(normalEye, lightDirN3), 0.0f) * lightColor3;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = att*specularStrength * specCoeff * lightColor2;

    vec3 reflectDir2 = reflect(-lightDirN2, normalEye);
    float specCoeff2 = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular2 = att2*specularStrength * specCoeff * lightColor;

    vec3 reflectDir3 = reflect(-lightDirN3, normalEye);
    float specCoeff3 = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular3 = att3*specularStrength * specCoeff * lightColor3;
}
void computeLightComponents()
{	
   
       vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
    //	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse3 = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), 32);
	specular3 = specularStrength * specCoeff * lightColor;
}
void main() 
{ 
    //computeLightComponents();
    computeDirLight();
   
    //compute final vertex color
    float fogFactor = computeFog();
    
    vec4 fogColor=vec4(0.5f,0.5f,0.5f,1.0f);
    vec4   sandColor=vec4(1.3f,1.0f,0.7f,1.0f);
	float shadow = computeShadow();
    vec3 color = min((ambient + (1.0f - shadow)*(diffuse + diffuse2+diffuse3)) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow)*(specular +specular2+specular3)* texture(specularTexture, fTexCoords).rgb, 1.0f);
    if(fog==1)
    fColor = fogColor * (1 - fogFactor) +  vec4(color, transparent)* fogFactor;
else if(sandstorm==1){
    float sandFactor = computeSandstorm();
    fColor = sandColor * (1 - fogFactor) +  vec4(color, transparent)* fogFactor;}
    else
    fColor = vec4(color, transparent);
}
