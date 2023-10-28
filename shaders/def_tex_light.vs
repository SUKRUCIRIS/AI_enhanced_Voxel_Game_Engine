#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
out vec2 texCoord;
out vec3 normal;
uniform mat4 camera;
uniform mat4 model;
uniform mat4 normalMatrix;
void main(){
    gl_Position = camera * vec4(vec3(model * vec4(pos, 1.0f)), 1.0f);
	normal=normalize(norm * mat3(normalMatrix));
	texCoord = tex;
}