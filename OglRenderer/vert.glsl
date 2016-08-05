#version 440 core

layout(location = 1) uniform float time;
layout(location = 2) uniform mat4 camera;
layout(location = 3) uniform mat4 meshTransform;

layout(location = 0)in vec3  vertexPos_;
layout(location = 1)in vec3  vertexNormal_;
layout(location = 2)in vec2  uvCoord_;

out vec3 frag;

void main() {
    vec3 displacement = vec3(sin(time * .001), 0.0f, sin(time*0.003)) * 40.0f;
    frag = vec3(vertexNormal_);
    gl_Position = camera * meshTransform * vec4(vertexPos_ + displacement, 1.0);
}
