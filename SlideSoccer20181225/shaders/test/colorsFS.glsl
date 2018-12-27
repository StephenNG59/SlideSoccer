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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

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

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// ambient
//	vec3 ambient = light.ambient * vec3(texture(material.ambient[0], TexCoords));
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * material.diffuse;

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
//	vec3 reflectDir = reflect(-lightDir, norm);
//	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//	vec3 specular = light.specular * (spec * material.specular);
//	vec3 specular = light.specular * material.specular;

	// emission
	vec3 emission = vec3(0.0f);
	for (int i = 0; i < emissionTexNum; i++)
	{
		emission += vec3(texture(material.emissionTex[i], TexCoords));
	}


	vec3 result;
	for(int i = 0; i < NR_LIGHT_NUM; i++)
	{
		if (pointLights[i].isExist)
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
		if (dirLights[i].isExist)
			result += CalcDirLight(dirLights[i], norm, viewDir);
		if (spotLights[i].isExist)
			result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
	}

//	vec3 result = ambient + diffuse + specular + emission;

	result += emission;

	FragColor = vec4(result, 1.0);
//	FragColor = vec4(FragPos.bbb, 1.0);
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
		ambientMat += vec3(texture(material.ambientTex[i], TexCoords));
	}
	ambientMat += material.ambient;
    vec3 ambient = light.ambient * ambientMat;

	vec3 diffuseMat = vec3(0.0f);
	for (int i = 0; i < diffuseTexNum; i++)
	{
		diffuseMat += vec3(texture(material.diffuseTex[i], TexCoords));
	}
	diffuseMat += material.diffuse;
    vec3 diffuse = light.diffuse * (diff * diffuseMat);
	// #NOTE - remember to add the factor diff!!!!(and spec below too)

	vec3 specularMat = vec3(0.0f);
	for (int i = 0; i < specularTexNum; i++)
	{
		specularMat += vec3(texture(material.specularTex[i], TexCoords));
	}
	specularMat += material.specular;
    vec3 specular = light.specular * (spec * specularMat);

    return (ambient + diffuse + specular);
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
		ambientMat += vec3(texture(material.ambientTex[i], TexCoords));
	}
	ambientMat += material.ambient;
    vec3 ambient = light.ambient * ambientMat;

	vec3 diffuseMat = vec3(0.0f);
	for (int i = 0; i < diffuseTexNum; i++)
	{
		diffuseMat += vec3(texture(material.diffuseTex[i], TexCoords));
	}
	diffuseMat += material.diffuse;
    vec3 diffuse = light.diffuse * diff * diffuseMat;

	vec3 specularMat = vec3(0.0f);
	for (int i = 0; i < specularTexNum; i++)
	{
		specularMat += vec3(texture(material.specularTex[i], TexCoords));
	}
	specularMat += material.specular;
    vec3 specular = light.specular * spec * specularMat;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
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
		ambientMat += vec3(texture(material.ambientTex[i], TexCoords));
	}
	ambientMat += material.ambient;
    vec3 ambient = light.ambient * ambientMat;

	vec3 diffuseMat = vec3(0.0f);
	for (int i = 0; i < diffuseTexNum; i++)
	{
		diffuseMat += vec3(texture(material.diffuseTex[i], TexCoords));
	}
	diffuseMat += material.diffuse;
    vec3 diffuse = light.diffuse * diff * diffuseMat;

	vec3 specularMat = vec3(0.0f);
	for (int i = 0; i < specularTexNum; i++)
	{
		specularMat += vec3(texture(material.specularTex[i], TexCoords));
	}
	specularMat += material.specular;
    vec3 specular = light.specular * spec * specularMat;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}