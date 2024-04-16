#include <vector>
#include <glad.h> 

struct RawModel {
    GLuint VAO;
    int vertexCount;
    
    // The origin for the model's vertices
    int x;
    int y;
    int z;
    
    RawModel(): VAO(0) {}
};

class Loader {
private:
    std::vector<GLuint> vaos;
    std::vector<GLuint> vbos;
    std::vector<GLuint> textures; // Note: Consider using GLuint for textures as well

public:
    Loader() {};

    RawModel loadToVAO(std::vector<float> positions, int dimensions) {
        RawModel model;
        model.vertexCount = positions.size() / dimensions;
        glGenVertexArrays(1, &model.VAO);
        vaos.push_back(model.VAO);
        glBindVertexArray(model.VAO);

        // Store data in attribute array, via the VBO
        GLuint VBO;
        glGenBuffers(1, &VBO);
        vbos.push_back(VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);

        // Configure vertex position attribute
        glVertexAttribPointer(0, dimensions, GL_FLOAT, GL_FALSE, dimensions * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        return model;
    }
};