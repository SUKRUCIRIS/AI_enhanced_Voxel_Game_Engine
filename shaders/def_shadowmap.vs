#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(pos, 1.0);
}