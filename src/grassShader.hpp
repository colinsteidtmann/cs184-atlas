#include "shader.h"
#include "camera.h"


class GrassShader : public Shader
{
private:
    static const std::string VERTEX_FILE;
    static const std::string FRAGMENT_FILE;
    static const std::string GEOMETRY_FILE;
    

public:
    unsigned int ID;
    unsigned int vertexShaderID;
    unsigned int fragmentShaderID;
    unsigned int geometryShaderID;

    GrassShader(const char *vertexFilePath, const char *fragmentFilePath, const char *geometryFilePath) : Shader(vertexFilePath, fragmentFilePath, geometryFilePath){}


    void loadProjectionMatrix(const glm::mat4 &projection)
    {
        this->setMat4("matrices.projMatrix", projection);
    }

    void loadViewMatrix(Camera &camera)
    {
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        this->setMat4("matrices.viewMatrix", viewMatrix);
    }

    void loadModelMatrix(const glm::mat4 &modelMatrix)
    {
        // loadMatrix(location_modelMatrix, modelMatrix);
        this->setMat4("matrices.modelMatrix", modelMatrix);
    }
};
