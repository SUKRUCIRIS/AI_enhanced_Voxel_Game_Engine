#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
in vec3 crntPos;
uniform sampler2D tex0;

uniform vec4 lightColor;
uniform vec3 lightDir;
uniform float ambient;
uniform float specularStrength;
uniform vec3 camPos;
uniform float shininess;

void main(){
	vec3 lightDirection = normalize(lightDir);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), shininess);
	float specular = specAmount * specularStrength;

	FragColor = texture(tex0, texCoord) * lightColor * (diffuse + ambient + specular);

	FragColor.a = 1.0f;
}