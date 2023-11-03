#version 400 core
out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D tex0;
void main(){
	FragColor = texture(tex0, texCoord);
	if(FragColor.a<1.0){
		discard;
	}
}