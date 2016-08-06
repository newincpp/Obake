#version 410 core

uniform float uTime;
//uniform mat4 uCamera;
//uniform mat4 uMeshTransform;

in vec3 vInfVertexPos_;
in vec3 vInfVertexNormal_;
in vec2 vInfUvCoord_;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;

void main() {
    // Store the fragment position vector in the first gbuffer texture
    gPosition = vInfVertexPos_;
    // Also store the per-fragment normals into the gbuffer
    gNormal = vInfVertexNormal_;
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = vec3(vInfUvCoord_, 0.0f);
    //gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    //// Store specular intensity in gAlbedoSpec's alpha component
    //gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}
