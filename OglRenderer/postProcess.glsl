#version 440 core

layout(location = 1) uniform float time;
layout(location = 2) uniform mat4 camera;
layout(location = 3) uniform mat4 transformation;

layout(location = 4) uniform sampler2D gPosition;
layout(location = 5) uniform sampler2D gNormal;
layout(location = 6) uniform sampler2D gAlbedoSpec;

in vec2 TexCoords;

out vec4 outColour;

void main() { 
    //vec3 cp = texture(gNormal, TexCoords).rgb; 
    vec3 cp = vec3(TexCoords, 0.0f);
    vec3 cn = texture(gNormal, TexCoords).rgb; 
    outColour = vec4(mix(cp, cn, sin(time * 0.001f)), 1.0f);
}
