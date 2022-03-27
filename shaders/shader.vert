#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inuv;
layout(location = 3) in vec3 bitangents;
layout(location = 4) in vec3 tangents;
layout(location = 5) in vec3 normal;


layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat4 model;
} push;

void main() {
    gl_Position = ubo.proj * ubo.view * push.model * vec4(inPosition, 1.0);
    fragColor = inColor;

}