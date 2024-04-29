#version 410 core
out vec4 FragColor;
 
in GS_OUT {
	vec2 textCoord;
} fs_in;
 
uniform sampler2D u_texture1;
 
void main(){
	vec4 color = texture(u_texture1, fs_in.textCoord);
	if (color.a < 0.05) discard;
	FragColor = color;
}