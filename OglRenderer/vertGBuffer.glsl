#version 440 core
layout(location = 1) uniform float time;
layout(location = 2) uniform mat4 camera;
layout(location = 3) uniform mat4 meshTransform;

layout(location = 0) in vec3  vertexPos_;
layout(location = 1) in vec3  vertexNormal_;
layout(location = 2) in vec2  uvCoord_;

out vec3 gPosition;
out vec3 gNormal;
out vec4 gAlbedoSpec;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    gPosition = vertexPos_;
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(vertexNormal_);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = vec3(uvCoord_,0.0f);
    //gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    //// Store specular intensity in gAlbedoSpec's alpha component
    //gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}  
