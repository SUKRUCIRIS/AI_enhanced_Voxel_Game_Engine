#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
out vec2 texCoord;
uniform mat4 camera;
uniform mat4 model;
void main(){
    gl_Position = camera * vec4(vec3(model * vec4(pos, 1.0f)), 1.0);
	texCoord = tex;
}