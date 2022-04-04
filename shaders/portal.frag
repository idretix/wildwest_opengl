#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 lightPosEye;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 portalColor;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
vec3 diffuse3;

in vec4 fragPosLightSpace;
uniform sampler2D shadowMap;

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
	specular = specularStrength * specCoeff * lightColor;
}
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


void computeDirLight()
{
   	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin

    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction

    vec3 lightDirN = vec3(normalize(lightPosEye.xyz - fPosEye.xyz));

    float constant = 1.0f;
    float linear = 0.35f; float quadratic = 0.44f;
    float dist = length(lightPosEye.xyz - fPosEye.xyz);
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = att*ambientStrength * portalColor;

    //compute diffuse light
    diffuse = att*max(dot(normalEye, lightDirN), 0.0f) * portalColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = att*specularStrength * specCoeff * portalColor;
  
}

void main() 
{ 
  //  computeLightComponents();
    computeDirLight();
   
    //compute final vertex color
    float fogFactor = computeFog();
    vec4 fogColor=vec4(0.5f,0.5f,0.5f,1.0f);
	float shadow = computeShadow();
    vec3 color = min((ambient + (1.0f - shadow)*(diffuse)) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow)*(specular)* texture(specularTexture, fTexCoords).rgb, 1.0f);
    //fColor = fogColor * (1 - fogFactor) +  vec4(color, 1.0f)* fogFactor;

    fColor = vec4(color, 0.7f);
}
