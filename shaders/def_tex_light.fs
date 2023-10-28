#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
uniform sampler2D tex0;

uniform vec4 lightColor;
uniform vec3 lightDir;
uniform float ambient;

void main(){
	vec3 lightDirection = normalize(lightDir);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	FragColor = texture(tex0, texCoord) * (lightColor * diffuse + ambient);

	FragColor.a = 1.0f;
}