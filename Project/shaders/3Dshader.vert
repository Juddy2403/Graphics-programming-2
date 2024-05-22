#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 4) in vec3 inColor;
layout(location = 6) in vec2 inTexCoord;
layout(location = 8) in vec3 inTangent;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec3 fragTangent;
layout(location = 4) out vec3 fragPosition;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    vec4 tNormal =  ubo.model*vec4(inNormal,0);
    fragNormal = normalize(tNormal.xyz); // interpolation of normal attribute in fragment shader.
    fragColor = inColor; // interpolation of color attribute in fragment shader.
    fragTexCoord = inTexCoord; // interpolation of texCoord attribute in fragment shader.
    //fragTangent = normalize(ubo.model*inTangent); // interpolation of tangent attribute in fragment shader.
    vec4 tan =  ubo.model*vec4(inTangent,0);
    fragTangent = normalize(tan.xyz); // interpolation of normal attribute in fragment shader.
    fragPosition = gl_Position.xyz; // interpolation of position attribute in fragment shader.
}