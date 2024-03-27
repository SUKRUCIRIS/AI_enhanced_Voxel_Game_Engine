#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
layout(location = 3) in float text_id;

out vec2 texCoord;
out vec3 normal;
out vec3 crntPos;
out vec2 coord;
flat out float texture_id;

uniform mat4 camera;
uniform mat4 model;
uniform mat4 normalMatrix;

uniform float time;

float mapValue(float value, float inMin, float inMax, float outMin, float outMax)
{
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

void main(){
	crntPos = vec3(model * vec4(pos, 1.0f));
  float wave=mapValue(sin(time+crntPos.x+crntPos.z),-1,1,0,0.17);
  crntPos.y=crntPos.y+wave;
  gl_Position = camera * vec4(crntPos, 1.0f);
  coord=gl_Position.xy/gl_Position.w;
  coord = (coord + 1.0) * 0.5;
	normal = normalize(norm * mat3(normalMatrix));
	texCoord = tex;
	texture_id = text_id;
}