#version 410 core
layout (location = 0) in vec3 aPos;

/*
out VS_OUT {
    
} vs_out;
*/

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform bool u_grassEnabled;
void main() {
	vec4 liftedPosition = vec4(aPos.x, aPos.y + .05, aPos.z, 1.0);
	gl_Position = liftedPosition; 
}