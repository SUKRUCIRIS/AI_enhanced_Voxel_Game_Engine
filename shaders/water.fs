#version 400 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gTexCoordo;

in vec2 texCoord;
in vec3 normal;
in vec3 crntPos;
in vec2 coord;
flat in float texture_id;

uniform sampler2D textures[32];

uniform sampler2D gTexCoordcopy;

uniform float time;

float mapValue(float value, float inMin, float inMax, float outMin, float outMax)
{
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

void main(){
  gPosition=crntPos;
  gNormal=normal;
  vec4 rgb=texture(textures[int(texture_id)], texCoord);
  float wave=0;
  wave=mapValue(sin(time+crntPos.x+crntPos.y+crntPos.z),-1,1,-0.002,0.002);
  vec4 old=texture(gTexCoordcopy, coord+vec2(wave));
  vec3 result = vec3(rgb.a) * rgb.rgb + vec3(1.0 - rgb.a) * old.rgb;
  gTexCoordo=result;
}