#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

#include "camera.h"
#include "loader.h"
#include "grassShader.hpp"
#include "main.h"

class GrassRenderer
{
private:
    GrassShader shader;
    Loader loader;
    void unbind();
    unsigned int texture1;

    public : GrassRenderer(Loader loader, GrassShader grass_shader, glm::mat4 projectionMatrix);
    void render(Camera camera);
    void loadProjectionMatrix(glm::mat4 projectionMatrix);
    RawModel *quad_chunks;
    void setUpVAO(std::vector<float> grass_vertices, int idx);
    unsigned int loadTextureFromFile(const char *path);
    std::string getCurrentWorkingDirectory();
};
