#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
layout(location = 3) in mat4 model;
layout(location = 7) in mat4 normalMatrix;
layout(location = 11) in float text_id;

void main()
{
    gl_Position = model * vec4(pos, 1.0);
}