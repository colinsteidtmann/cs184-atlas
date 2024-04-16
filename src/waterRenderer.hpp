#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

#include "camera.h"
#include "loader.cpp"
#include "waterShader.hpp"
#include "main.h"

struct Water {
  float x;
  float z;  
  float height;
  float x_length;
  float z_length;

  Water (float _x, float _z, float _height, float _x_length, float _z_length) : x(_x), z(_z), height(_height), x_length(_x_length), z_length(_z_length) {}
};

class WaterRenderer {
    private:
        WaterShader shader;
        Loader loader;
        void prepareRender(Camera camera);
        void unbind();

    public:
        WaterRenderer(Loader loader, WaterShader water_shader, glm::mat4 projectionMatrix);
        void render(Camera camera);
        void loadProjectionMatrix(glm::mat4 projectionMatrix);
        RawModel* quad_chunks;
        void setUpVAO(std::vector<float> water_vertices, int idx, float x, float y, float z);
};

