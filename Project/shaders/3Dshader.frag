#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragTangent;
layout(location = 4) in vec3 fragPosition;

layout(push_constant) uniform pushConstants{
    vec3 cameraPos;
    int usingNormalMap;
    int shadingMode;
    int isPBREnabled;
} pushConstantsBlock;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalTexSampler;
layout(binding = 3) uniform sampler2D glossTexSampler;
layout(binding = 4) uniform sampler2D specularTexSampler;

const vec3 lightDirection = normalize(vec3(0.577f, -0.577f, 0.577f));

#define LAMBER_MODE 0
#define NORMAL_MODE 1
#define SPECULAR_MODE 2
#define COMBINED_MODE 3

vec3 Phong(float specularReflectance, float phongExp, vec3 viewDir, vec3 normal)
{
    vec3 specularRefl = vec3(specularReflectance, specularReflectance, specularReflectance);
    if (phongExp == 0) return specularRefl;
    vec3 reflectVar = reflect(-lightDirection, normal);
    float cos = clamp(dot(reflectVar, -viewDir), 0.0, 1.0);
    return specularRefl * pow(cos, phongExp);
}

vec3 RemapToUnitRange(vec3 v) {
    // Find the maximum value in the vector
    float maxValue = max(v.x, max(v.y, v.z));
    // only if the max value is greater than 1 i ll remap
    if (maxValue > 1.0) {
        return v / maxValue;
    }
    return v;
}

void main() {
    const float gPI = 3.14;
    const float gLightIntensity = 7.0f;
    const float gShininess = 25.0f;

    const bool usingNormalMap = pushConstantsBlock.usingNormalMap == 1;
    vec3 ambient = vec3(0.003f, 0.003f, 0.003f);

    // Sampling color from maps
    float glossinesMapSample = texture(glossTexSampler, fragTexCoord).r;
    float specularMapSample = texture(specularTexSampler, fragTexCoord).r;
    vec3 diffuseMapSample = texture(texSampler, fragTexCoord).rgb;

    vec3 normal;
    if (usingNormalMap) {
        vec3 normalMapSample = texture(normalTexSampler, fragTexCoord).rgb;
        // Remapping the value from [0,1] to [-1,1]
        normal =  2 * normalMapSample - vec3(1, 1, 1);
        // Transforming it in the correct tangent space
        vec3 binormal = cross(fragNormal, fragTangent);
        mat3 tangentSpaceAxis = mat3(fragTangent, binormal, fragNormal);
        normal = normalize(tangentSpaceAxis * normal);
    }
    else {
        normal = normalize(fragNormal);
    }

    float lightDirCos = dot(normal, -(lightDirection));
    if (lightDirCos < 0) {
        outColor = vec4(0.f, 0.f, 0.f, 1.f);
        return;
    }

    switch (pushConstantsBlock.shadingMode)
    {
    case LAMBER_MODE:
        outColor = vec4(RemapToUnitRange(lightDirCos * diffuseMapSample), 1.0);
        break;
    case NORMAL_MODE:
        outColor = vec4(normal, 1.0);
        return;
    case SPECULAR_MODE:
        {
            vec3 viewDir = normalize(pushConstantsBlock.cameraPos - fragPosition);
            vec3 specular = Phong(specularMapSample, glossinesMapSample * gShininess, viewDir, normal);
            outColor = vec4(RemapToUnitRange(specular), 1.0);
            return;
        }
    case COMBINED_MODE:
        {
            if (pushConstantsBlock.isPBREnabled == 0) {
                outColor = vec4(RemapToUnitRange(lightDirCos * diffuseMapSample), 1.0);
                return;
            }
            vec3 viewDir = normalize(pushConstantsBlock.cameraPos - fragPosition);
            vec3 specular = Phong(specularMapSample, glossinesMapSample * gShininess, viewDir, normal);
            vec3 diffuse = diffuseMapSample * gLightIntensity / gPI;
            vec3 finalColor = lightDirCos * diffuse + specular + ambient;
            finalColor = RemapToUnitRange(finalColor);
            outColor = vec4(finalColor, 1.0);
            return;
        }
    }
}