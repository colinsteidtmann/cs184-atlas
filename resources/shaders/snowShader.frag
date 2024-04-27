#version 330 core

in vec3 Color;
in float height;

out vec4 FragColor;

void main() {
    // Define the snow threshold
    float snowThreshold = 0.5; // Adjust this value as needed

    // If the height is above the snow threshold, render snow; otherwise, render terrain
    if (height > snowThreshold) {
        // Apply snowy texture
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color for snow
    }
    else {
        // Render terrain
        // You can add your terrain shading logic here
        // For example, you might use a texture sampler to render terrain with different textures based on height
        // FragColor = texture(some_terrain_texture, some_uv_coordinates);
        // FragColor = vec4(0.5, 0.5, 0.5, 1.0); // Example: Render gray color for terrain
        FragColor = vec4(Color, 1.0);
    }
}