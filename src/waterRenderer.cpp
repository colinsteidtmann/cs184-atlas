#include "glad.h"
#include "glm/fwd.hpp"
#include "waterRenderer.hpp"
#include <limits.h>
#include <limits>

WaterRenderer::WaterRenderer(Loader loader, WaterShader waterShader, glm::mat4 projectionMatrix) : shader(waterShader) {
    shader.use();
    shader.loadProjectionMatrix(projectionMatrix);
    this->loader = loader;
    quad_chunks = new RawModel[xMapChunks * yMapChunks];
    // shader.stop();
    // setUpVAO(loader);
}

glm::mat4 createTransformationMatrix(glm::vec3 translation, float rotationX, float rotationY, float rotationZ, float x_scale, float z_scale) {
    // Create an identity matrix
    glm::mat4 matrix = glm::mat4(1.0f);
    
    // Apply translation
    matrix = glm::translate(matrix, translation);
    
    // Apply rotations (in radians)
    matrix = glm::rotate(matrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Apply scaling
    matrix = glm::scale(matrix, glm::vec3(x_scale, 1, z_scale)); // This tells us how much to stretch the water tile.
    
    return matrix;
}

void WaterRenderer::render(Camera camera) {
    prepareRender(camera);	

    // Similar code to the main's render function
    // Measures number of map chunks away from origin map chunk the camera is
    gridPosX = (int)(camera.Position.x - originX) / chunkWidth + xMapChunks / 2;
    gridPosY = (int)(camera.Position.z - originY) / chunkHeight + yMapChunks / 2;
    
    // Render map chunks
    for (int y = 0; y < yMapChunks; y++) {
        for (int x = 0; x < xMapChunks; x++) {
            RawModel quad = quad_chunks[x + y*xMapChunks];

            // Only render chunk if it's within render distance AND if there's water at that VAO
            if (std::abs(gridPosX - x) <= chunk_render_distance 
                && (y - gridPosY) <= chunk_render_distance
                && quad.VAO != 0) {

                glBindVertexArray(quad.VAO);
                glEnableVertexAttribArray(0);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-chunkWidth / 2.0 + (chunkWidth - 1) * x, 0.0, -chunkHeight / 2.0 + (chunkHeight - 1) * y));
                shader.setMat4("u_model", model);

                // Water chunk
                glBindVertexArray(quad.VAO);
                glm::mat4 modelMatrix = createTransformationMatrix(
                glm::vec3(quad.x, quad.y, quad.z), 
                0, 0, 0, 
                1, 1);
                shader.loadModelMatrix(modelMatrix);
                // glEnable(GL_POLYGON_OFFSET_FILL);
                // float factor = 10.0f;
                // float units = 10.0f;
                // glPolygonOffset(factor, units);
                // glDisable(GL_POLYGON_OFFSET_FILL);
                glDrawArrays(GL_TRIANGLES, 0, quad.vertexCount);
            }
        }
    }
    
}

void WaterRenderer::loadProjectionMatrix(glm::mat4 projectionMatrix) {
    shader.loadProjectionMatrix(projectionMatrix);
}

void WaterRenderer::prepareRender(Camera camera){
    shader.use();
    shader.loadViewMatrix(camera);
}

void WaterRenderer::unbind(){
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void WaterRenderer::setUpVAO(std::vector<float> water_vertices, int idx, float x, float y, float z) {
    // Just x and z vectex positions here, y is set to 0 in v.shader
    // We have all the water vertices, now make a quad out of them using the min/max of the x and z.
    RawModel quad = loader.loadToVAO(water_vertices, 2);
    quad.x = x;
    quad.y = y;
    quad.z = z;
    quad_chunks[idx] = quad;
}

// Add a update projection matrix function here. Rather than reinstantiation the renderer every time
