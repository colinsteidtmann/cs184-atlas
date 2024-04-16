#version 330 core
in vec2 position;

out vec2 textureCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;


void main(void) {

	gl_Position = u_projection * u_view * u_model * vec4(position.x, 0, position.y, 1.0);
	textureCoords = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5);
 
}