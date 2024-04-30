#version 330 core
in vec2 position;

out vec4 clip_space;
out vec2 textureCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

const float tiling = 6.0;

void main(void) {

	clip_space = u_projection * u_view * u_model * vec4(position.x, 0, position.y, 1.0);
	gl_Position = clip_space;
	textureCoords = vec2(position.x/127.0/10.0 + 0.5, position.y/127.0/10.0 + 0.5) * tiling; // Normalize, then make it from 0 -> 1?
 
}