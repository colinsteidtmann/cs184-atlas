#include "camera.h"
#include "shader.h"

class SnowyGrassShader : public Shader {
private:
  static const std::string VERTEX_FILE;
  static const std::string FRAGMENT_FILE;
  static const std::string GEOMETRY_FILE;

public:
  unsigned int ID;
  unsigned int vertexShaderID;
  unsigned int fragmentShaderID;
  unsigned int geometryShaderID;

  SnowyGrassShader(const char *vertexFilePath, const char *fragmentFilePath,
              const char *geometryFilePath)
      : Shader(vertexFilePath, fragmentFilePath, geometryFilePath) {}

  void loadProjectionMatrix(const glm::mat4 &projection) {
    this->setMat4("u_projection", projection);
  }

  void loadViewMatrix(Camera &camera) {
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    this->setMat4("u_view", viewMatrix);
  }

  void loadModelMatrix(const glm::mat4 &modelMatrix) {
    this->setMat4("u_model", modelMatrix);
  }
};
