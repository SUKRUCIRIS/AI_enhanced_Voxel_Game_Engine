#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
layout(location = 3) in mat4 model;
layout(location = 7) in mat4 normalMatrix;
layout(location = 11) in float text_id; 

out vec2 texCoord;
out vec3 normal;
out vec3 crntPos;
flat out int texture_id;

uniform mat4 camera;

void main(){
	crntPos = vec3(model * vec4(pos, 1.0f));
  gl_Position = camera * vec4(crntPos, 1.0f);
	normal = normalize(norm * mat3(normalMatrix));
	texCoord = tex;
	texture_id = int(text_id);
}