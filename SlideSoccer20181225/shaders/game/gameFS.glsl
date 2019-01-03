#version 330 core

#define NR_TEX_NUM 3
#define NR_LIGHT_NUM 3

out vec4 FragColor;

struct Material {
	sampler2D ambientTex[NR_TEX_NUM];
	sampler2D diffuseTex[NR_TEX_NUM];
	sampler2D specularTex[NR_TEX_NUM];
	sampler2D emissionTex[NR_TEX_NUM];
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirLight {
	bool isExist;

    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
	bool isExist;

    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
	bool isExist;

    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};


in GS_FS {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

//in vec3 FragPos;
//in vec3 Normal;
//in vec2 TexCoords;
//in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform PointLight pointLights[NR_LIGHT_NUM];
uniform DirLight dirLights[NR_LIGHT_NUM];
uniform SpotLight spotLights[NR_LIGHT_NUM];

uniform int ambientTexNum = 0;
uniform int diffuseTexNum = 0;
uniform int specularTexNum = 0;
uniform int emissionTexNum = 0;

uniform vec3 shadowLightPos;
uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);


void main()
{
	// ambient
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(light.position - fs_in.FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * material.diffuse;

	// specular
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	// emission
	vec3 emission = vec3(0.0f);
	for (int i = 0; i < emissionTexNum; i++)
	{
		emission += vec3(texture(material.emissionTex[i], fs_in.TexCoords));
	}

	vec3 result;
	for(int i = 0; i < NR_LIGHT_NUM; i++)
	{
		if (pointLights[i].isExist)
			result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);
		if (dirLights[i].isExist)
			result += CalcDirLight(dirLights[i], norm, viewDir);
		if (spotLights[i].isExist)
			result += CalcSpotLight(spotLights[i], norm, fs_in.FragPos, viewDir);
	}

	result += emission;
	
	// Calculate shadow
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
	result = result * (1.0 - shadow);
//	result = vec3(fs_in.FragPosLightSpace);

	FragColor = vec4(result, 1.0);
}


// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
	vec3 ambientMat = vec3(0.0f);
	for (int i = 0; i < ambientTexNum; i++)
	{
		ambientMat += vec3(texture(material.ambientTex[i], fs_in.TexCoords));
	}
	if (ambientTexNum == 0)
		ambientMat += material.ambient;
    vec3 ambient = light.ambient * ambientMat;

	vec3 diffuseMat = vec3(0.0f);
	for (int i = 0; i < diffuseTexNum; i++)
	{
		diffuseMat += vec3(texture(material.diffuseTex[i], fs_in.TexCoords));
	}
	if (diffuseTexNum == 0)
		diffuseMat += material.diffuse;
    vec3 diffuse = light.diffuse * diff * diffuseMat;

	vec3 specularMat = vec3(0.0f);
	for (int i = 0; i < specularTexNum; i++)
	{
		specularMat += vec3(texture(material.specularTex[i], fs_in.TexCoords));
	}
	if (specularTexNum == 0)
		specularMat += material.specular;
    vec3 specular = light.specular * spec * specularMat;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
	vec3 ambientMat = vec3(0.0f);
	for (int i = 0; i < ambientTexNum; i++)
	{
		ambientMat += vec3(texture(material.ambientTex[i], fs_in.TexCoords));
	}
	if (ambientTexNum == 0)
		ambientMat += material.ambient;
    vec3 ambient = light.ambient * ambientMat;

	vec3 diffuseMat = vec3(0.0f);
	for (int i = 0; i < diffuseTexNum; i++)
	{
		diffuseMat += vec3(texture(material.diffuseTex[i], fs_in.TexCoords));
	}
	if (diffuseTexNum == 0)
		diffuseMat += material.diffuse;
    vec3 diffuse = light.diffuse * (diff * diffuseMat);
	// #NOTE - remember to add the factor diff!!!!(and spec below too)

	vec3 specularMat = vec3(0.0f);
	for (int i = 0; i < specularTexNum; i++)
	{
		specularMat += vec3(texture(material.specularTex[i], fs_in.TexCoords));
	}
	if (specularTexNum == 0)
		specularMat += material.specular;
    vec3 specular = light.specular * (spec * specularMat);

    return (ambient + diffuse + specular);
}


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
	vec3 ambientMat = vec3(0.0f);
	for (int i = 0; i < ambientTexNum; i++)
	{
		ambientMat += vec3(texture(material.ambientTex[i], fs_in.TexCoords));
	}
	if (ambientTexNum == 0)
		ambientMat += material.ambient;
    vec3 ambient = light.ambient * ambientMat;

	vec3 diffuseMat = vec3(0.0f);
	for (int i = 0; i < diffuseTexNum; i++)
	{
		diffuseMat += vec3(texture(material.diffuseTex[i], fs_in.TexCoords));
	}
	if (diffuseTexNum == 0)
		diffuseMat += material.diffuse;
    vec3 diffuse = light.diffuse * diff * diffuseMat;

	vec3 specularMat = vec3(0.0f);
	for (int i = 0; i < specularTexNum; i++)
	{
		specularMat += vec3(texture(material.specularTex[i], fs_in.TexCoords));
	}
	if (specularTexNum == 0)
		specularMat += material.specular;
    vec3 specular = light.specular * spec * specularMat;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(shadowLightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}