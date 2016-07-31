#version 440 core

layout(location = 1) uniform float time;
layout(location = 2) uniform mat4 transformation;

in vec3 frag;

out vec4 outColor;

void main()
{
    outColor = vec4(frag, 0.0f);
//    outColor = vec4(sin(time), sin(time), sin(time*10), 1.0);
}
