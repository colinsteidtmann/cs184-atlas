#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <glad.h>

struct RawModel
{
    GLuint VAO;
    int vertexCount;
    int x, y, z;
    RawModel() : VAO(0) {}
};

class Loader
{
private:
    std::vector<GLuint> vaos;
    std::vector<GLuint> vbos;
    std::vector<GLuint> textures;

public:
    Loader() {} // Default constructor declaration
    RawModel loadToVAO(std::vector<float> positions, int dimensions);
};

#endif // LOADER_H