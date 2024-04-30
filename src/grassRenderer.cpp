#include "grassRenderer.hpp"
#include "../lib/stb_image.hpp"
#include "glad.h"
#include "glm/fwd.hpp"
#define STB_IMAGE_IMPLEMENTATION

GrassRenderer::GrassRenderer(Loader loader, GrassShader grass_shader,
                             glm::mat4 projectionMatrix)
    : shader(grass_shader) {
  this->loader = loader;
  quad_chunks = new RawModel[xMapChunks * yMapChunks];
  std::string path =
      getCurrentWorkingDirectory() + "/resources/textures/grass_texture.png";
  this->texture1 = loadTextureFromFile(path.c_str());
  glPointSize(10.0f);
  glEnable(GL_PROGRAM_POINT_SIZE);
  shader.use();
  glUniform1i(glGetUniformLocation(shader.ID, "u_textgrass"), 0);
  shader.loadProjectionMatrix(projectionMatrix);
  float distance = std::max(chunkWidth, chunkHeight) * chunk_render_distance;
  shader.setFloat("u_distance", distance);
  shader.setSampler2D("u_wind", 1);
}

void GrassRenderer::unbind() {
  int numVao = yMapChunks * xMapChunks;
  for (int i = 0; i < numVao; i++) {
    glDeleteVertexArrays(1, &quad_chunks[i].VAO);
  }
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
  glUseProgram(0);
}

void GrassRenderer::render(Camera camera) {
  shader.use();
  shader.loadViewMatrix(camera);
  shader.setFloat("u_time", glfwGetTime());
  shader.setVec3("u_cameraPosition", camera.Position);
  shader.setBool("u_grassEnabled", GRASS_ENABLED);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);

  gridPosX = (int)(camera.Position.x - originX) / chunkWidth + xMapChunks / 2;
  gridPosY = (int)(camera.Position.z - originY) / chunkHeight + yMapChunks / 2;
  for (int y = 0; y < yMapChunks; y++) {
    for (int x = 0; x < xMapChunks; x++) {
      RawModel quad = quad_chunks[x + y * xMapChunks];
      // Only render chunk if it's within render distance
      if (std::abs(gridPosX - x) <= chunk_render_distance &&
          (y - gridPosY) <= chunk_render_distance) {

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(
            model, glm::vec3(-chunkWidth / 2.0 + (chunkWidth - 1) * x, 0.0,
                             -chunkHeight / 2.0 + (chunkHeight - 1) * y));
        shader.loadModelMatrix(model);

        glBindVertexArray(quad.VAO);
        glDrawArrays(GL_POINTS, 0, quad.vertexCount);
      }
    }
  }
}

void GrassRenderer::loadProjectionMatrix(glm::mat4 projectionMatrix) {
  shader.loadProjectionMatrix(projectionMatrix);
}

void GrassRenderer::setUpVAO(std::vector<float> grass_vertices, int idx) {
  // TODO: Implement setUpVAO
  RawModel quad = loader.loadToVAO(grass_vertices, 3);
  quad_chunks[idx] = quad;
}

unsigned int GrassRenderer::loadTextureFromFile(const char *path) {
  // std::string filename = std::string(path);
  std::string filename = std::string(path); // directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  stbi_set_flip_vertically_on_load(false);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

#ifdef __linux__
#include <unistd.h>
#define GetCurrentDir getcwd
#elif defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#define GetCurrentDir _getcwd
#elif defined(__APPLE__)
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string GrassRenderer::getCurrentWorkingDirectory() {
  char cCurrentPath[FILENAME_MAX];
  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
    return std::string();
  }
  cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
  std::string currentPath = std::string(cCurrentPath);
  int found = currentPath.find_last_of("/\\");
  currentPath = currentPath.substr(0, found);
  unsigned int i = 0;
  while (i < currentPath.size()) {
    if (currentPath[i] == '\\') {
      currentPath[i] = '/';
    }
    ++i;
  }
  // std::cout << currentPath << std::endl;
  return currentPath;
}