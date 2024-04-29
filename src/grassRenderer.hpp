#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "grassShader.hpp"
#include "loader.h"
#include "main.h"

class GrassRenderer {
public:
  void unbind();

private:
  GrassShader shader;
  Loader loader;

  unsigned int texture1;

public:
  GrassRenderer(Loader loader, GrassShader grass_shader,
                glm::mat4 projectionMatrix);
  void render(Camera camera);
  void loadProjectionMatrix(glm::mat4 projectionMatrix);
  RawModel *quad_chunks;
  void setUpVAO(std::vector<float> grass_vertices, int idx);
  unsigned int loadTextureFromFile(const char *path);
  std::string getCurrentWorkingDirectory();
};
