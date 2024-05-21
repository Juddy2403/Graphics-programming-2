#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragTangent;

layout(push_constant) uniform viewDir {
    vec3 viewDir;
} viewDirBlock;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalTexSampler;
layout(binding = 3) uniform sampler2D glossTexSampler;
layout(binding = 4) uniform sampler2D specularTexSampler;

const vec3 lightDirection = normalize(vec3(0.5, -0.5, 0.5));

vec3 Phong(float specularReflectance, float phongExp, vec3 viewDir, vec3 normal)
{
    vec3 specularRefl = vec3(specularReflectance, specularReflectance, specularReflectance);
    if (phongExp == 0) return specularRefl;
    vec3 reflectVar = reflect(lightDirection, normal);
    float cos = clamp(dot(reflectVar, viewDir), 0.0, 1.0);
    if (phongExp == 1) return specularRefl * cos;
    if (phongExp == 2) return specularRefl * cos * cos;
    return specularRefl * pow(cos, phongExp);
}

void main() {
    const float gPI = 3.14;
    const float gLightIntensity = 7.0f;
    const float gShininess = 25.0f;
    vec3 ambient = vec3(0.03f, 0.03f, 0.03f);

    //Normal mapping
    vec3 normalMapSample = texture(normalTexSampler, fragTexCoord).rgb;
    // Remapping the value from [0,1] to [-1,1]
    normalMapSample = 2 * normalMapSample - 1;
    // Transforming it in the correct tangent space
    vec3 binormal = cross(fragNormal, fragTangent);
    mat3 tangentSpaceAxis = mat3(fragTangent, binormal, fragNormal);
    normalMapSample = normalMapSample * tangentSpaceAxis;

    float lightDirCos = dot(normalMapSample, -lightDirection);

    //if (lightDirCos >= 0)
    //{
    // Sampling color from maps
    float glossinesMapSample = texture(glossTexSampler, fragTexCoord).r;
    float specularMapSample = texture(specularTexSampler, fragTexCoord).r;
    // SpecularColor sampled from SpecularMap and PhongExponent from GlossinessMap
    vec3 specular = Phong(specularMapSample, glossinesMapSample * gShininess, viewDirBlock.viewDir, normalMapSample);
    vec3 cd = texture(texSampler, fragTexCoord).rgb;
    vec3 diffuse = cd * gLightIntensity / gPI;
    outColor = vec4(lightDirCos * diffuse + specular + ambient, 1.0);
    //} else {
    //outColor = vec4(0.f, 0.f, 0.f, 1.f);
    //}
}