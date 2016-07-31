#version 440 core

layout(location = 1) uniform float time;
layout(location = 2) uniform mat4 camera;

layout(location = 0)in vec3  vertexPos_;
layout(location = 1)in vec3  vertexNormal_;
layout(location = 2)in vec2  uvCoord_;

out vec3 frag;

void main()
{
    vec3 displacement = vec3(sin(time * .001), 0.0f, sin(time*0.003));
    frag = vec3(0.0f,0.0f,1.0f); 
    vec3 lol = vertexNormal_;
    frag = vec3(uvCoord_,0.0f);
    gl_Position = camera * vec4(vertexPos_ + displacement, 1.0);
}
