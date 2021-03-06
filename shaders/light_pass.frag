#version 430 core

uniform sampler2D gPositionMap;
uniform sampler2D gNormalMap;
uniform sampler2D gAlbedoMap;

uniform samplerCube depthMap;

uniform vec3 gScreenSize;
uniform vec3 gEyePosition;

uniform float farPlane;

out vec4 FragColor;

struct DirLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

struct PointLight {
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
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

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

const float Epsilon = 0.0000001;

subroutine vec3 LightType(vec3, vec3, vec3, vec4);
subroutine uniform LightType getLightColor;

subroutine float Shadows(vec3, vec3);
subroutine uniform Shadows getShadowValue;

subroutine (Shadows)
float ShadowsDisabled(vec3 fragPos, vec3 lightPos) {
    return 0.0;
}

subroutine (Shadows)
float ShadowsEnabled(vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;

    float closestDepth = texture(depthMap, fragToLight).r;

    closestDepth *= farPlane;

    float currentDepth = length(fragToLight);

    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

subroutine (LightType)
vec3 UnknownLightType(vec3 worldPos, vec3 worldNormal, vec3 viewDir, vec4 albedo) {
    return albedo.xyz * vec3(0.0, 1.0, 0.0);
}

subroutine (LightType)
vec3 DirLightType(vec3 worldPos, vec3 worldNormal, vec3 viewDir, vec4 albedo) {
	vec3 lightDir = normalize(-dirLight.direction);
	vec3 reflectDir = normalize(reflect(-lightDir, worldNormal));
	float diff = max(dot(worldNormal, lightDir), 0.0);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 100.0);

	vec3 ambient = dirLight.ambient * albedo.xyz;
	vec3 diffuse = dirLight.diffuse * diff * albedo.xyz;
	vec3 specular = dirLight.specular * spec * albedo.w;

	return ambient + diffuse + specular;
}

subroutine (LightType)
vec3 PointLightType(vec3 worldPos, vec3 worldNormal, vec3 viewDir, vec4 albedo) {
    vec3 lightDir = normalize(pointLight.position - worldPos);
	vec3 reflectDir = normalize(reflect(-lightDir, worldNormal));
	float diff = max(dot(worldNormal, lightDir), 0.0);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 100.0);
	float distance = length(pointLight.position - worldPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	vec3 ambient  = pointLight.ambient * albedo.xyz * attenuation;
	vec3 diffuse  = pointLight.diffuse * diff * albedo.xyz * attenuation;
	vec3 specular = pointLight.specular * spec * albedo.w * attenuation;

    float shadow = getShadowValue(worldPos, pointLight.position);

    ambient = ambient * shadow;

    return ambient + diffuse + specular;
}

subroutine (LightType)
vec3 SpotLightType(vec3 worldPos, vec3 worldNormal, vec3 viewDir, vec4 albedo) {
	vec3 lightDir = normalize(spotLight.position - worldPos);
	vec3 reflectDir = normalize(reflect(-lightDir, worldNormal));
	float diff = max(dot(worldNormal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 0.01);
	float distance = length(spotLight.position - worldPos);
	float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));
	float theta = dot(lightDir, normalize(spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - (spotLight.outerCutOff / epsilon)), 0.0, 1.0);

	vec3 ambient = spotLight.ambient * albedo.xyz * attenuation * intensity;
	vec3 diffuse = spotLight.diffuse * diff * albedo.xyz * attenuation * intensity;
	vec3 specular = spotLight.specular * spec * albedo.w * attenuation * intensity;

	return ambient + diffuse + specular;
}

vec2 calcTexCoord() {
    return gl_FragCoord.xy / gScreenSize.xy;
}

void main() {
    vec2 texCoord = calcTexCoord();

    vec3 position = texture(gPositionMap, texCoord).xyz;
    vec4 albedo = texture(gAlbedoMap, texCoord).xyzw;
    vec3 normal = texture(gNormalMap, texCoord).xyz;

    vec3 viewDir = normalize(gEyePosition - position);
    vec3 light = getLightColor(position, normal, viewDir, albedo);

    FragColor = vec4(light, 0.0);
}