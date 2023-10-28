#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
in vec3 crntpos;
uniform sampler2D tex0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform float ambient;
uniform float specularLight;

void main(){
	vec3 lightDirection = normalize(lightPos - crntpos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	vec3 viewDirection = normalize(camPos - crntpos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

	FragColor = texture(tex0, texCoord) * lightColor * (diffuse + ambient + specular);

	FragColor.a=1.0f;
}