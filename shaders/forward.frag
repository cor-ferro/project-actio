#version 430 core

#define NUM_DIR_LIGHTS 8
#define NUM_POINT_LIGHTS 8
#define NUM_SPOT_LIGHTS 8
#define SHININESS_FACTOR 3

const float Epsilon = 0.0000001;

struct DirLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};

uniform DirLight dirLights[NUM_DIR_LIGHTS];
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];

uniform int countDirLights;
uniform int countPointLights;
uniform int countSpotLights;

uniform vec2 resolution;
uniform mat4 model;

in vec3 normal;
in vec3 normal1;
in vec3 fragmentPosition;
in vec2 texCoord;
in mat3 TBN;

out vec4 FragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	vec3 position;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D heightTexture;
uniform sampler2D normalTexture;
uniform samplerCube cubeTexture;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), Epsilon + material.shininess);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, texCoord));
	vec3 specular = light.specular * spec * (vec3(texture(specularTexture, texCoord)));
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), Epsilon + material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient  = light.ambient  * vec3(texture(diffuseTexture, texCoord));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseTexture, texCoord));
	vec3 specular = light.specular * spec * vec3(texture(specularTexture, texCoord));
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
} 

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
	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, texCoord));
	vec3 specular = light.specular * spec * vec3(texture(specularTexture, texCoord));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}

void main()
{
	vec3 norm = normalize(normal);

	norm = texture(heightTexture, texCoord).rgb;
	norm = normalize(normal * 2.0 - 1.0);
	norm = normalize(TBN * normal);

	vec3 viewDir = normalize(viewPos - fragmentPosition);
	vec3 result = vec3(0.0);

	for(int i = 0; i < countDirLights; i++)
		result += CalcDirLight(dirLights[i], norm, viewDir);

	// for(int i = 0; i < countPointLights; i++)
	// 	result += CalcPointLight(pointLights[i], norm, fragmentPosition, viewDir);
		
	// for(int i = 0; i < countSpotLights; i++)
	// 	result += CalcSpotLight(spotLights[i], norm, fragmentPosition, viewDir);

	//result+= material.ambient + material.diffuse + material.specular;

	vec3 skyboxReflect = reflect(viewDir, normalize(normal));

	result = mix(
		result,
		texture(cubeTexture, -skyboxReflect).rgb * 0.5,
		min(1.0, (max(material.shininess, 0.0) / 1000.0))
	);

	FragColor = vec4(result, 1.0);
}