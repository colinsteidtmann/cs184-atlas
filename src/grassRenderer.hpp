#include "grassTexture.h"
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
    GrassTexture grassTexture;
    GrassShader shader;
    Loader loader;
    void unbind();
    const float fAlphaTest = 0.25f;
    const float fAlphaMultiplier = 1.5f;

public:
    GrassRenderer(Loader loader, GrassShader grass_shader, glm::mat4 projectionMatrix);
    void render(Camera camera);
    void loadProjectionMatrix(glm::mat4 projectionMatrix);
    RawModel *quad_chunks;
    void setUpVAO(std::vector<float> grass_vertices, int idx);
};
