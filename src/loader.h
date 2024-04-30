// #ifndef LOADER_H
// #define LOADER_H
#pragma once
#include <vector>
#include <glad.h>

struct RawModel {
    GLuint VAO;
    int vertexCount;
    int numObjects;

    // The origin for the model's vertices
    int x;
    int y;
    int z;

    RawModel(): VAO(0) {}
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
    GLuint loadTexture(const std::string& fileName);
};

//#endif // LOADER_H