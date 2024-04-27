#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec3 aOffset;

uniform mat4 u_model1;

out vec3 Color;
out float height;

void main() {
//    vec3 Normal = aNormal;
//    //    vec3 Normal = transpose(inverse(mat3(u_model))) * aNormal;
//
//    vec3 lighting = calculateLighting(Normal, fragPos);
//    Color = aColor * lighting;
//
//    gl_Position = u_projection * u_view * u_model1 * vec4(aPos + aOffset, 1.0);
    height = vec3(u_model1 * vec4(aPos + aOffset, 1.0)).y;
    Color = aColor;
}