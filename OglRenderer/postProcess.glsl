#version 410 core

uniform float uTime;
//uniform mat4 uCamera;
//uniform mat4 uMeshTransform;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

in vec2 TexCoords;

out vec3 outColour;

void main() { 
    vec2 distCoord = TexCoords * 2.0f;
    vec3 cp = texture(gPosition, distCoord).rgb;  // normal
    vec3 cn = texture(gNormal, distCoord - vec2(1.0f,0.0f)).rgb;   // diffuse
    vec3 ca = texture(gAlbedoSpec, distCoord - vec2(0.0f,1.0f)).rgb; // 
    outColour = cn + cp + ca;
}
