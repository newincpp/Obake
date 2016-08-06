#version 410 core

uniform float uTime;
//uniform mat4 uCamera;
//uniform mat4 uMeshTransform;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

in vec2 TexCoords;

out vec4 outColour;

void main() { 
    //color = vec4(sin(uTime * 0.001f), 0.3f, 0.3f, 1.0f);
    vec3 cp = vec3(0.2f, 0.3f, 0.2f);
    //vec3 cp = texture(gNormal, TexCoords).rgb; 
    vec3 cn = texture(gNormal, TexCoords).rgb; 
    outColour = vec4(mix(cp, cn, sin(uTime * 0.001f)), 1.0f);
}
