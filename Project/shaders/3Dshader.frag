#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalTexSampler;

const vec3 lightDirection = normalize(vec3(0.5, -0.5, 0.5));

vec3 Phong(float specularReflectance, float phongExp, vec3 viewDir, vec3 normal)
{
    vec3 specularRefl = vec3(specularReflectance, specularReflectance, specularReflectance);
    if (phongExp == 0) return specularRefl;
    vec3 reflectVar = reflect(lightDirection,normal);
    float cos = clamp(dot(reflectVar, viewDir), 0.0, 1.0);
    if (phongExp == 1) return specularRefl * cos;
    if (phongExp == 2) return specularRefl * cos * cos;
    return specularRefl * pow(cos, phongExp);
}

void main() {

    // Calculate the dot product between the normal and the light direction
    float lightDirCos = max(dot(fragNormal, -lightDirection), 0.1);

    vec3 normalMapSample = texture(normalTexSampler, fragTexCoord).rgb;
    // Remapping the value from [0,1] to [-1,1]
    normalMapSample = 2 * normalMapSample - 1;
    // Transforming it in the correct tangent space
    //float3 binormal = cross(input.Normal, input.Tangent);
    //float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);
    //normalMapSample = mul(normalMapSample, tangentSpaceAxis);

    // Simple diffuse lighting
    vec3 diffuse = lightDirCos * fragColor;// Assuming white light

    // Output color
    outColor = vec4(diffuse * normalMapSample, 1.0);
}