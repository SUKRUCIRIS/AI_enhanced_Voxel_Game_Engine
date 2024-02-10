#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
layout(location = 3) in float text_id;

out vec2 texCoord;
out vec3 normal;
out vec3 crntPos;
out float specular;
out float diffuse;
out float fogmult;
flat out int texture_id;

uniform mat4 camera;
uniform mat4 model;
uniform mat4 normalMatrix;

uniform vec3 lightDir;
uniform float specularStrength;

uniform vec3 camPos;
uniform mat4 view;

uniform float fog_start;
uniform float fog_end;

void main(){
	crntPos = vec3(model * vec4(pos, 1.0f));
  gl_Position = camera * vec4(crntPos, 1.0f);
	normal = normalize(norm * mat3(normalMatrix));
	texCoord = tex;
	texture_id = int(text_id);

	vec3 lightDirection = normalize(lightDir);
	diffuse = max(dot(normal, lightDirection), 0.0f);

	specular=0;
	if(diffuse!=0){
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 2);
		specular = specAmount * specularStrength;
	}

	float dist = distance(vec3(crntPos.x,0,crntPos.z), vec3(camPos.x,0,camPos.z));

	if(dist >= fog_start){
		if(dist >= fog_end){
			fogmult = 1;
		}
		else{
			fogmult = (dist - fog_start) / (fog_end - fog_start);
		}
	}
	else{
		fogmult = 0;
	}
}