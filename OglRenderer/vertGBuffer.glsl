#version 440 core
layout(location = 1) uniform float time;
layout(location = 2) uniform mat4 camera;
layout(location = 3) uniform mat4 meshTransform;

layout(location = 0) in vec3  vertexPos_;
layout(location = 1) in vec3  vertexNormal_;
layout(location = 2) in vec2  uvCoord_;

out vec3 vInfVertexPos_;
out vec3 vInfVertexNormal_;
out vec2 vInfUvCoord_;

void main()
{    
    vInfVertexPos_ = vertexPos_;
    vInfVertexNormal_ = vertexNormal_;
    vInfUvCoord_ = uvCoord_;
}  
