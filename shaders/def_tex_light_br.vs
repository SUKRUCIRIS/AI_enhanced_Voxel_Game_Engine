#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
layout(location = 3) in float text_id;
out vec2 texCoord;
out vec3 normal;
out vec3 crntPos;
out vec4 fragPosLight;
flat out int texture_id;
uniform mat4 camera;
uniform mat4 lightProjection;
void main(){
	crntPos = pos;
    gl_Position = camera * vec4(crntPos, 1.0f);
	fragPosLight = lightProjection * vec4(crntPos, 1.0f);
	normal = normalize(norm);
	texCoord = tex;
	texture_id = int(text_id);
}