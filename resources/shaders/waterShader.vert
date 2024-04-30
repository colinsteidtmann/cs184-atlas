#version 330 core
in vec2 position;

out vec4 clip_space;
out vec2 textureCoords;
out vec3 to_camera_vector;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform vec3 camera_position;

const float tiling = 6.0;

void main(void) {
	vec4 world_position = u_model * vec4(position.x, 0, position.y, 1.0);
	clip_space = u_projection * u_view * world_position;
	gl_Position = clip_space;
	// float plane_width = 127 * 3; // xMapChunks * chunkWidth;
	// float plane_height = 127 * 3; // yMapChunks * chunkHeight;
	textureCoords = vec2(position.x/127.0/10.0 + 0.5, position.y/127.0/10.0 + 0.5) * tiling; // Normalize, then make it from 0 -> 1?
	to_camera_vector = camera_position - world_position.xyz;
 
}