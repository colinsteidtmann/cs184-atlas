#include "glad.h"
#include "glm/fwd.hpp"
#include "grassRenderer.hpp"

GrassRenderer::GrassRenderer(Loader loader, GrassShader grass_shader, glm::mat4 projectionMatrix) : shader(grass_shader), grassTexture("../resources/textures/grassPack.dds"){
    shader.use();
    shader.loadProjectionMatrix(projectionMatrix);
    this->loader=loader;
    quad_chunks = new RawModel[xMapChunks * yMapChunks];
    grassTexture.LoadTexture2D();
}

void GrassRenderer::unbind() {
    // TODO: Implement unbind
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void GrassRenderer::render(Camera camera) {
    grassTexture.BindTexture(0);
    shader.use();
    shader.loadViewMatrix(camera);
    shader.setFloat("fAlphaTest", fAlphaTest);
    shader.setFloat("fAlphaMultiplier", fAlphaMultiplier);
    shader.setVec4("vColor", glm::vec4(1, 1, 1, 1));
    shader.setFloat("gSampler", 0);
    shader.setFloat("fTimePassed", 0);
    

    gridPosX = (int)(camera.Position.x - originX) / chunkWidth + xMapChunks / 2;
    gridPosY = (int)(camera.Position.z - originY) / chunkHeight + yMapChunks / 2;

    for (int y = 0; y < yMapChunks; y++)
    {
        for (int x = 0; x < xMapChunks; x++)
        {
            RawModel quad = quad_chunks[x + y * xMapChunks];
            // Only render chunk if it's within render distance
            if (std::abs(gridPosX - x) <= chunk_render_distance && (y - gridPosY) <= chunk_render_distance)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(-chunkWidth / 2.0 + (chunkWidth - 1) * x, 0.0, -chunkHeight / 2.0 + (chunkHeight - 1) * y));
                shader.loadModelMatrix(model);
                glBindVertexArray(quad.VAO);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_POINTS, 0, quad.vertexCount);
            }
        }
    }
}

void GrassRenderer::loadProjectionMatrix(glm::mat4 projectionMatrix) {
    shader.loadProjectionMatrix(projectionMatrix);
}

void GrassRenderer::setUpVAO(std::vector<float> grass_vertices, int idx)
{
    // TODO: Implement setUpVAO
    RawModel quad = loader.loadToVAO(grass_vertices, 3);
    quad_chunks[idx] = quad;
}