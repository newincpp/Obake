#version 440 core

layout(location = 1) uniform float time;
layout(location = 2) uniform sampler2D gPosition;
layout(location = 3) uniform sampler2D gNormal;
layout(location = 4) uniform sampler2D gAlbedoSpec;

in vec2 TexCoords;

out vec4 color;

void main() { 
    //color = vec4(sin(time * 0.001f), 0.3f, 0.3f, 1.0f);
    vec3 cp = texture(gNormal, TexCoords).rgb; 
    vec3 cn = texture(gNormal, TexCoords).rgb; 
    color = vec4(mix(cp, cn, sin(time * 0.001f)), 1.0f);
}
