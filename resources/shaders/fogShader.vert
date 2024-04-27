#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec3 aOffset;

out vec3 Color;
out vec3 fragPos;
out float height;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform vec3 u_viewPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

vec3 calculateLighting(vec3 Normal, vec3 FragPos) {
    // Ambient lighting
    vec3 ambient = light.ambient;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = light.diffuse * diff;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = light.specular * spec;
    
    return (ambient + diffuse + specular);
}

void main() {
    fragPos = vec3(u_model * vec4(aPos + aOffset, 1.0));
    vec3 Normal = aNormal;
//    vec3 Normal = transpose(inverse(mat3(u_model))) * aNormal;

    vec3 lighting = calculateLighting(Normal, fragPos);
    float meshHeight = 32;
//    if (aColor.x > 0.5 || aColor.y > 0.8 || aColor.z > 0.8) {
//        Color = vec3(1.0, 1.0, 1.0) * lighting;
//    } else {
//        Color = aColor * lighting;
//    }
    // Color = aColor * lighting;

    float snowThreshold = 0.8; // Adjust this value as needed

    // If the height is above the snow threshold, render snow; otherwise, render terrain
    if (aPos.y > snowThreshold * meshHeight) {
        // Apply snowy texture
        Color = vec3(1.0, 1.0, 1.0) * lighting; // White color for snow
    } else {
        Color = aColor * lighting;
    }

    height = fragPos.y;
    gl_Position = u_projection * u_view * u_model * vec4(aPos + aOffset, 1.0);
}
