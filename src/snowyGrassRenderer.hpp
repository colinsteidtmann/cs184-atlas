#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "snowyGrassShader.hpp"
#include "loader.h"
#include "main.h"

class SnowyGrassRenderer {
public:
  void unbind();

private:
  SnowyGrassShader shader;
  Loader loader;

  unsigned int texture1;

public:
  SnowyGrassRenderer(Loader loader, SnowyGrassShader snowy_grass_shader,
                glm::mat4 projectionMatrix);
  void render(Camera camera);
  void loadProjectionMatrix(glm::mat4 projectionMatrix);
  RawModel *quad_chunks;
  void setUpVAO(std::vector<float> snowy_grass_vertices, int idx);
  unsigned int loadTextureFromFile(const char *path);
  std::string getCurrentWorkingDirectory();
};
